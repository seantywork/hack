#include "af-rxtx-core.h"
#include "af-rxtx.h"



struct bpool_params bpool_params_default = {
	.n_buffers = 64 * 1024,
	.buffer_size = XSK_UMEM__DEFAULT_FRAME_SIZE,
	.mmap_flags = 0,

	.n_users_max = 16,
	.n_buffers_per_slab = XSK_RING_PROD__DEFAULT_NUM_DESCS * 2,
};

struct xsk_umem_config umem_cfg_default = {
	.fill_size = XSK_RING_PROD__DEFAULT_NUM_DESCS * 2,
	.comp_size = XSK_RING_CONS__DEFAULT_NUM_DESCS,
	.frame_size = XSK_UMEM__DEFAULT_FRAME_SIZE,
	.frame_headroom = XSK_UMEM__DEFAULT_FRAME_HEADROOM,
	.flags = 0,
};

struct port_params port_params_default = {
	.xsk_cfg = {
		.rx_size = XSK_RING_CONS__DEFAULT_NUM_DESCS,
		.tx_size = XSK_RING_PROD__DEFAULT_NUM_DESCS,
		.libxdp_flags = 0,
		.xdp_flags = XDP_FLAGS_DRV_MODE,
		.bind_flags = XDP_USE_NEED_WAKEUP,
	},

	.bp = NULL,
	.iface = NULL,
	.iface_queue = 0,
};



struct bpool_params bpool_params;
struct xsk_umem_config umem_cfg;
struct bpool *bp;

struct port_params port_params[MAX_PORTS];
struct port *ports[MAX_PORTS];
u64 n_pkts_rx[MAX_PORTS];
u64 n_pkts_tx[MAX_PORTS];
int n_ports;

pthread_t threads[MAX_THREADS];
struct thread_data thread_data[MAX_THREADS];
int n_threads;

int quit;

static void
signal_handler(int sig)
{
	quit = 1;
}

static void remove_xdp_program(void)
{
	struct xdp_multiprog *mp;
	int i, err;

	for (i = 0 ; i < n_ports; i++) {
	        mp = xdp_multiprog__get_from_ifindex(if_nametoindex(port_params[i].iface));
	        if (!mp) {
	        	printf("No XDP program loaded on %s\n", port_params[i].iface);
	        	continue;
	        }

                err = xdp_multiprog__detach(mp);
                if (err)
                        printf("Unable to detach XDP program: %s\n", strerror(-err));
	}
}

int main(int argc, char **argv)
{
	struct timespec time;
	u64 ns0;
	int i;

	/* Parse args. */
	memcpy(&bpool_params, &bpool_params_default,
	       sizeof(struct bpool_params));
	memcpy(&umem_cfg, &umem_cfg_default,
	       sizeof(struct xsk_umem_config));
	for (i = 0; i < MAX_PORTS; i++)
		memcpy(&port_params[i], &port_params_default,
		       sizeof(struct port_params));

	if (parse_args(argc, argv)) {
		print_usage(argv[0]);
		return -1;
	}

	/* Buffer pool initialization. */
	bp = bpool_init(&bpool_params, &umem_cfg);
	if (!bp) {
		printf("Buffer pool initialization failed.\n");
		return -1;
	}
	printf("Buffer pool created successfully.\n");

	/* Ports initialization. */
	for (i = 0; i < MAX_PORTS; i++)
		port_params[i].bp = bp;

	for (i = 0; i < n_ports; i++) {
		ports[i] = port_init(&port_params[i]);
		if (!ports[i]) {
			printf("Port %d initialization failed.\n", i);
			return -1;
		}
		print_port(i);
	}
	printf("All ports created successfully.\n");

	/* Threads. */
	for (i = 0; i < n_threads; i++) {
		struct thread_data *t = &thread_data[i];
		u32 n_ports_per_thread = n_ports / n_threads, j;

		for (j = 0; j < n_ports_per_thread; j++) {
			t->ports_rx[j] = ports[i * n_ports_per_thread + j];
			t->ports_tx[j] = ports[i * n_ports_per_thread +
				(j + 1) % n_ports_per_thread];
		}

		t->n_ports_rx = n_ports_per_thread;

		print_thread(i);
	}

	for (i = 0; i < n_threads; i++) {
		int status;

		status = pthread_create(&threads[i],
					NULL,
					thread_func,
					&thread_data[i]);
		if (status) {
			printf("Thread %d creation failed.\n", i);
			return -1;
		}
	}
	printf("All threads created successfully.\n");

	/* Print statistics. */
	signal(SIGINT, signal_handler);
	signal(SIGTERM, signal_handler);
	signal(SIGABRT, signal_handler);

	clock_gettime(CLOCK_MONOTONIC, &time);
	ns0 = time.tv_sec * 1000000000UL + time.tv_nsec;
	for ( ; !quit; ) {
		u64 ns1, ns_diff;

		sleep(1);
		clock_gettime(CLOCK_MONOTONIC, &time);
		ns1 = time.tv_sec * 1000000000UL + time.tv_nsec;
		ns_diff = ns1 - ns0;
		ns0 = ns1;

		print_port_stats_all(ns_diff);
	}

	/* Threads completion. */
	printf("Quit.\n");
	for (i = 0; i < n_threads; i++)
		thread_data[i].quit = 1;

	for (i = 0; i < n_threads; i++)
		pthread_join(threads[i], NULL);

	for (i = 0; i < n_ports; i++)
		port_free(ports[i]);

	bpool_free(bp);

	remove_xdp_program();

	return 0;
}