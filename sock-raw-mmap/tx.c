
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

static struct sockaddr_ll txring_daddr;
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
    memset(&txring_daddr, 0, sizeof(txring_daddr));

    txring_daddr.sll_family	  = AF_PACKET;
    txring_daddr.sll_protocol = SOCKADDR_PROTOCOL;
    txring_daddr.sll_ifindex  = ifindex;
    txring_daddr.sll_halen	  = ETH_ALEN; 
    memset(&txring_daddr.sll_addr, 0xFF, ETH_ALEN);

    memcpy(&dest_daddr, &txring_daddr, sizeof(dest_daddr));

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



static unsigned char EcatPacket[] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,
                                     0x00, 0x00, 0x88, 0xa4, 0x0e, 0x10, 0x07, 0x80, 0x00, 0x00,
                                     0x30, 0x01, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

static int do_tx()
{
    int	  status = 1;
    char* txRing;
    int	  txFd;
    char  *pkt;
    int	  len;
    int tx_mmap = 1;

    txFd = init_packetsock(&txRing, PACKET_TX_RING, tx_mmap);
    if (txFd < 0){
        return 1;
    }

    

    if (bind(txFd, (struct sockaddr*)&txring_daddr, sizeof(txring_daddr)) != 0)
    {
        perror("bind");
        return -1;
    }
    


    {
        int	  offset = 0;
        char* pkt	 = NULL;

        process_tx(txFd, txRing, EcatPacket, sizeof(EcatPacket), offset, tx_mmap);
        sleep(1);

    }

    if (exit_packetsock(txFd, txRing, tx_mmap))
        return 1;


    return 0;
}



int main(int argc, char** argv)
{
    if (argc > 1)
        Ethernet_ifname = argv[1];

    printf("using interface: %s\n", Ethernet_ifname);

    int result = do_tx();

    printf("tx: done: %d\n", result);
}