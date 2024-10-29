
#include <stdarg.h>
#define _GNU_SOURCE
#define __USE_GNU
#include <pthread.h>
#include <sched.h>
#include <inttypes.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <poll.h>

#include <arpa/inet.h>
#include <netinet/if_ether.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/stat.h>

#include <linux/if.h>
#include <linux/if_packet.h>

/// The number of frames in the ring
//  This number is not set in stone. Nor are block_size, block_nr or frame_size
#define CONF_RING_FRAMES 16
#define FRAME_SIZE 2048
#define CONF_DEVICE "eth0"

#define SOCK_PROTOCOL(ringtype) htons(0x88a4)
#define SOCKADDR_PROTOCOL htons(0x88a4)

/// Offset of data from start of frame
#define TX_DATA_OFFSET TPACKET_ALIGN(sizeof(struct tpacket2_hdr))
#define RX_DATA_OFFSET TX_DATA_OFFSET + 34

/// (unimportant) macro for loud failure
#define RETURN_ERROR(lvl, msg) \
    do                         \
    {                          \
        fprintf(stderr, msg);  \
        return lvl;            \
    } while (0);

void HandleError(const char* msg, int error)
{
    if (error != 0)
    {
        errno = error;
        perror(msg);
        _exit(error);
    }
}

void SetAffinity(int8_t cpu)
{
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(cpu, &cpuset);

    HandleError("pthread_set_affinity_np", pthread_setaffinity_np(pthread_self(), sizeof(cpuset), &cpuset));
}

static struct sockaddr_ll rxring_daddr;
static struct sockaddr_ll dest_daddr;
char*					  Ethernet_ifname = CONF_DEVICE;

/// create a linklayer destination address
//  @param ringdev is a link layer device name, such as "eth0"
static int init_ring_daddr(int fd, const char* ringdev, const int ringtype)
{
    struct ifreq ifr;
    int			 ifindex;

    // get device index
    strcpy(ifr.ifr_name, ringdev);
    if (ioctl(fd, SIOCGIFINDEX, &ifr))
    {
        perror("ioctl");
        return -1;
    }
    ifindex = ifr.ifr_ifindex;
    memset(&rxring_daddr, 0, sizeof(rxring_daddr));

    rxring_daddr.sll_family	  = AF_PACKET;
    rxring_daddr.sll_protocol = SOCKADDR_PROTOCOL;
    rxring_daddr.sll_ifindex  = ifindex;
    rxring_daddr.sll_halen	  = ETH_ALEN;  // e4:5f:01:6e:de:3d
    memset(&rxring_daddr.sll_addr, 0xFF, ETH_ALEN);

    memcpy(&dest_daddr, &rxring_daddr, sizeof(dest_daddr));

    return 0;
}

/// Initialize a packet socket ring buffer
//  @param ringtype is one of PACKET_RX_RING or PACKET_TX_RING
static char* init_packetsock_ring(int fd, int ringtype, int tx_mmap)
{
    struct tpacket_req tp;
    char*			   ring;
    int				   packet_version = TPACKET_V2;

    if (setsockopt(fd, SOL_PACKET, PACKET_VERSION, &packet_version, sizeof(packet_version)))
    {
        perror("setsockopt packet version");
        return NULL;
    }

    // tell kernel to export data through mmap()ped ring
    tp.tp_block_size = getpagesize();
    tp.tp_frame_size = FRAME_SIZE;
    tp.tp_frame_nr	 = CONF_RING_FRAMES;
    tp.tp_block_nr	 = (tp.tp_frame_nr * tp.tp_frame_size) / tp.tp_block_size;

    {
        if (init_ring_daddr(fd, Ethernet_ifname, ringtype))
            return NULL;
    }

    if (ringtype == PACKET_TX_RING & !tx_mmap)
    {
        return NULL;
    }

    if (setsockopt(fd, SOL_PACKET, ringtype, (void*)&tp, sizeof(tp)))
        RETURN_ERROR(NULL, "setsockopt() ring\n");

    // open ring
    ring = mmap(0, tp.tp_block_size * tp.tp_block_nr, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (ring == MAP_FAILED)
        RETURN_ERROR(NULL, "mmap()\n");

    return ring;
}

/// Create a packet socket. If param ring is not NULL, the buffer is mapped
//  @param ring will, if set, point to the mapped ring on return
//  @return the socket fd
static int init_packetsock(char** ring, int ringtype, int tx_mmap)
{
    int fd;

    // open packet socket
    fd = socket(PF_PACKET, SOCK_RAW, SOCK_PROTOCOL(ringtype));
    if (fd < 0)
        RETURN_ERROR(-1, "Root priliveges are required\nsocket() rx. \n");

    if (ring)
    {
        *ring = init_packetsock_ring(fd, ringtype, tx_mmap);

        if (!tx_mmap)
            return fd;

        if (!*ring)
        {
            // printf("Closing fd\n");
            close(fd);
            return -1;
        }
    }

    return fd;
}

static int exit_packetsock(int fd, char* ring, int tx_mmap)
{
    if (tx_mmap && munmap(ring, CONF_RING_FRAMES * FRAME_SIZE))
    {
        perror("munmap");
        return 1;
    }

    if (close(fd))
    {
        perror("close");
        return 1;
    }

    return 0;
}

/// transmit a packet using packet ring
//  NOTE: for high rate processing try to batch system calls,
//        by writing multiple packets to the ring before calling send()
//
//  @param pkt is a packet from the network layer up (e.g., IP)
//  @return 0 on success, -1 on failure
static int process_tx(int fd, char* ring, const char* pkt, size_t pktlen, int offset, int tx_mmap)
{
    static int ring_offset = 0;

    struct tpacket2_hdr* header;
    struct pollfd		 pollset;
    char*				 off;
    int					 ret;

    if (tx_mmap)
    {
        // fetch a frame
        // like in the PACKET_RX_RING case, we define frames to be a page long,
        // including their header. This explains the use of getpagesize().
        header = (void*)ring + (ring_offset * FRAME_SIZE);
        assert((((unsigned long)header) & (FRAME_SIZE - 1)) == 0);
        while (header->tp_status != TP_STATUS_AVAILABLE)
        {
            // if none available: wait on more data
            pollset.fd		= fd;
            pollset.events	= POLLOUT;
            pollset.revents = 0;
            ret				= poll(&pollset, 1, 1 /* don't hang */);
            if (ret < 0)
            {
                if (errno != EINTR)
                {
                    perror("poll");
                    return -1;
                }
                return 0;
            }
        }

        // fill data
        off = ((void*)header) + TX_DATA_OFFSET + offset;
        memcpy(off, pkt, pktlen);

        // fill header
        header->tp_len	  = pktlen;
        header->tp_status = TP_STATUS_SEND_REQUEST;

        // increase consumer ring pointer
        ring_offset = (ring_offset + 1) & (CONF_RING_FRAMES - 1);

        if (send(fd, NULL, 0, 0) < 0)
        // if (sendto(fd, NULL, 0, 0, (struct sockaddr*)&dest_daddr, sizeof(dest_daddr)) < 0)
        {
            perror("sendto");
            return -1;
        }
    }
    else
    {
        off = (void*)pkt;

        if (sendto(fd, off, pktlen, 0, (struct sockaddr*)&dest_daddr, sizeof(dest_daddr)) < 0)
        {
            perror("sendto");
            return -1;
        }
    }

    // printf("Tx:%d\n",ring_offset);fflush(stdout);

    return 0;
}

static void* process_rx(const int fd, char* rx_ring, int* len)
{
    volatile struct tpacket2_hdr* header;
    struct pollfd				  pollset;
    int							  ret;
    char*				 off;

    for (int i = 0; i < CONF_RING_FRAMES; i++)
    {
        // fetch a frame
        
        header = (void*)rx_ring + (i * FRAME_SIZE);
        assert((((unsigned long)header) & (FRAME_SIZE - 1)) == 0);
        if (header->tp_status & TP_STATUS_USER)
        {
            if (header->tp_status & TP_STATUS_COPY)
            {
                printf("copy\n");
                continue;
            }
            *len = header->tp_len;

            off = ((void*)header) + RX_DATA_OFFSET;

            for(int j = 0 ; j < *len; j++){

                if(j % 10 == 0){
                    printf("%02x \n", off[j]);
                } else {
                    printf("%02x ", off[j]);
                }
            }

            return (void*)header;
        }
    }
    return NULL;
}

// Release the slot back to the kernel
static void process_rx_release(char* packet)
{
    volatile struct tpacket2_hdr* header = (struct tpacket2_hdr*)packet;
    header->tp_status					 = TP_STATUS_KERNEL;
}

static void rx_flush(void* ring)
{
    for (int i = 0; i < CONF_RING_FRAMES; i++)
    {
        volatile struct tpacket2_hdr* hdr = ring + (i * FRAME_SIZE);
        hdr->tp_status					  = TP_STATUS_KERNEL;
    }
}


int do_rx()
{
    int	  status = 1;
    char *rxRing, *pkt;
    int	  rxFd;
    int	  len;

    rxFd = init_packetsock(&rxRing, PACKET_RX_RING, 1);
    if (rxFd < 0)
        return 1;


    if (bind(rxFd, (struct sockaddr*)&rxring_daddr, sizeof(rxring_daddr)) != 0)
    {
        perror("bind");
        return -1;
    }


    rx_flush(rxRing);

    {
        int	  offset = 0;
        char* pkt	 = NULL;

        rx_flush(rxRing);
        sleep(5);
        while (pkt = process_rx(rxFd, rxRing, &len))
        {
            printf("Rx [%d]:\n", len);
            process_rx_release(pkt);
        }
    }

    if (exit_packetsock(rxFd, rxRing, 1))
        return 1;


    return len;
}


int main(int argc, char** argv)
{
    if (argc > 1)
        Ethernet_ifname = argv[1];

    printf("using interface: %s\n", Ethernet_ifname);

    int result = do_rx();

    printf("rx: %d\n", result);

}