
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
#define CONF_DEVICE "veth01"

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


static uint8_t server_hw_addr[ETH_ALEN]= {
    0xaa,
    0xaa,
    0xaa,
    0x01,
    0x01,
    0x01,
};

static uint8_t client_hw_addr[ETH_ALEN]= {
    0xaa,
    0xaa,
    0xaa,
    0x02,
    0x02,
    0x02,
};

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

char*					  Ethernet_ifname = CONF_DEVICE;



static struct sockaddr_ll ring_daddr;
static struct sockaddr_ll dest_daddr;


/// create a linklayer destination address
//  @param ringdev is a link layer device name, such as "eth0"
static int init_ring_daddr(int fd, const char* ringdev, const int ringtype, struct sockaddr_ll* dest_daddr, uint8_t* hw_daddr)
{
    struct ifreq ifr;
    int			 ifindex;
    struct sockaddr_ll ring_daddr;
    // get device index
    strcpy(ifr.ifr_name, ringdev);
    if (ioctl(fd, SIOCGIFINDEX, &ifr))
    {
        perror("ioctl");
        return -1;
    }
    ifindex = ifr.ifr_ifindex;
    memset(&ring_daddr, 0, sizeof(ring_daddr));

    ring_daddr.sll_family	  = AF_PACKET;
    ring_daddr.sll_protocol = SOCKADDR_PROTOCOL;
    ring_daddr.sll_ifindex  = ifindex;
    ring_daddr.sll_halen	  = ETH_ALEN;  
    memcpy(&ring_daddr.sll_addr, hw_daddr, ETH_ALEN);

    memcpy(dest_daddr, &ring_daddr, sizeof(dest_daddr));

    return 0;
}




static char* init_packetsock_ring(int fd, int ringtype, int tx_mmap, struct sockaddr_ll* dest_daddr, uint8_t* hw_daddr)
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
        if (init_ring_daddr(fd, Ethernet_ifname, ringtype, dest_daddr, hw_daddr))
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

static int init_packetsock(char** ring, int ringtype, int tx_mmap, struct sockaddr_ll* dest_daddr, uint8_t* hw_daddr)
{
    int fd;

    // open packet socket
    fd = socket(PF_PACKET, SOCK_RAW, SOCK_PROTOCOL(ringtype));
    if (fd < 0)
        RETURN_ERROR(-1, "Root priliveges are required\nsocket() rx. \n");

    if (ring)
    {
        *ring = init_packetsock_ring(fd, ringtype, tx_mmap, dest_daddr, hw_daddr);

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


static int process_tx(int fd, char* ring, const char* pkt, size_t pktlen, int offset, int tx_mmap, struct sockaddr_ll* dest_daddr)
{
    static int ring_offset = 0;

    struct tpacket2_hdr* header;
    struct pollfd		 pollset;
    char*				 off;
    int					 ret;

    if (tx_mmap)
    {

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

        char* offrx = ((void*)pkt) + RX_DATA_OFFSET;

        // fill data
        off = ((void*)header) + TX_DATA_OFFSET + offset;
        memcpy(off, offrx, pktlen);

        // fill header
        header->tp_len	  = pktlen;
        header->tp_status = TP_STATUS_SEND_REQUEST;

        // increase consumer ring pointer
        ring_offset = (ring_offset + 1) & (CONF_RING_FRAMES - 1);

        if (send(fd, NULL, 0, 0) < 0)

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


void do_serve()
{
    int	  status = 1;
    char *txRing, *rxRing, *pkt;
    int   txFd;
    int	  rxFd;
    int	  len;

    struct sockaddr_ll txdest_daddr;
    struct sockaddr_ll rxdest_daddr;

    txFd = init_packetsock(&txRing, PACKET_TX_RING, 1, &txdest_daddr, client_hw_addr);
    if (txFd < 0){
        printf("failed to init tx packet sock\n");
        return;
    }

    rxFd = init_packetsock(&rxRing, PACKET_RX_RING, 1, &rxdest_daddr, server_hw_addr);
    if (rxFd < 0){
        printf("failed to init rx packet sock\n");
        return;
    }

    
    if (bind(txFd, (struct sockaddr*)&txdest_daddr, sizeof(txdest_daddr)) != 0)
    {
        printf("bind txfd\n");
        return;
    }

    if (bind(rxFd, (struct sockaddr*)&rxdest_daddr, sizeof(rxdest_daddr)) != 0)
    {
        printf("bind rxfd\n");
        return;
    }

    while(1){

        rx_flush(rxRing);

        {
            int	  offset = 0;
            char* pkt	 = NULL;

            while (pkt = process_rx(rxFd, rxRing, &len))
            {
                char* off = ((void*)pkt) + RX_DATA_OFFSET;

                printf("server RX: ");
                for(int i = 0 ; i < len; i ++){
                    if(i > 0 && i % 10 == 0){
                        printf("[%x] \n", off[i]);
                    } else {
                        printf("[%x] ", off[i]);
                    }
                }

                printf("\n");

                process_tx(txFd, txRing, pkt, len, offset, 1, &dest_daddr);

                printf("server TX done\n");

                process_rx_release(pkt);
            }
        }
    }


    if (exit_packetsock(rxFd, rxRing, 1))
        return;

    if (exit_packetsock(txFd, txRing, 1))
        return;


    return;
}


int main(int argc, char** argv)
{
    if (argc > 1)
        Ethernet_ifname = argv[1];

    printf("using interface: %s\n", Ethernet_ifname);

    do_serve();

}