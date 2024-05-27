#ifndef _DUMMETH_H_
#define _DUMMETH_H_



#include <linux/module.h>
#include <linux/init.h>
#include <linux/moduleparam.h>

#include <linux/sched.h>
#include <linux/kernel.h> /* printk() */
#include <linux/slab.h> /* kmalloc() */
#include <linux/errno.h>  /* error codes */
#include <linux/types.h>  /* size_t */
#include <linux/interrupt.h> 

#include <linux/in.h>
#include <linux/netdevice.h>   /* struct device, and other headers */
#include <linux/etherdevice.h> /* eth_type_trans */
#include <linux/ip.h>          /* struct iphdr */
#include <linux/tcp.h>         /* struct tcphdr */
#include <linux/skbuff.h>
#include <linux/version.h> 

#include <linux/in6.h>
#include <asm/checksum.h>

#include <linux/irqreturn.h>

#define DRV_NAME	"dummeth"

#define DUMMETH_TIMEOUT 5

#define mac_addr_len                    6 

#define ETH_RX_INTR 0x0001
#define ETH_TX_INTR 0x0002

extern int use_napi;
extern int timeout;
extern int pool_size;

extern struct net_device *device;
extern struct net_device_stats *stats;
extern struct napi_struct dummeth_napi_struct;

extern void (*dummeth_interrupt)(int, void *, struct pt_regs *);

extern struct header_ops ho; 
extern struct net_device_ops ndo;




struct eth_packet {
    struct eth_packet* next;
    struct net_device* dev;
    int datalen;
    u8 data[ETH_DATA_LEN];
};

struct eth_priv {
    struct net_device_stats stats;
    int status;
    struct eth_packet* ppool;
    struct eth_packet* rx_queue;
    int rx_int_enabled;
    int tx_packetlen;
    u8* tx_packetdata;
    struct sk_buff* skb;
    spinlock_t lock;
    struct net_device *dev;
    struct napi_struct napi;
};



int eth_header_create(struct sk_buff *skb, struct net_device *dev,
                unsigned short type, const void *daddr, const void *saddr,
                unsigned len);

void dummeth_setup_pool(struct net_device *dev);
void dummeth_teardown_pool(struct net_device *dev);
struct eth_packet *eth_dequeue_buf(struct net_device *dev);
void eth_release_buffer(struct eth_packet *pkt);

void dummeth_rx(struct net_device *dev, struct eth_packet *pkt);
void dummeth_rx_ints(struct net_device *dev, int enable);
int dummeth_napi_poll(struct napi_struct *napi, int budget);

void dummeth_regular_interrupt(int irq, void *dev_id, struct pt_regs *regs);

void dummeth_napi_interrupt(int irq, void *dev_id, struct pt_regs *regs);




int dummeth_open(struct net_device *dev);
int dummeth_close(struct net_device *dev);
netdev_tx_t dummeth_start_xmit(struct sk_buff *skb, struct net_device *dev);
int dummeth_do_ioctl(struct net_device *dev, struct ifreq *ifr, int cmd);
struct net_device_stats *dummeth_get_stats(struct net_device *dev);
int dummeth_config(struct net_device *dev, struct ifmap *map);
int dummeth_change_mtu(struct net_device *dev, int new_mtu);

#if LINUX_VERSION_CODE < KERNEL_VERSION(5,6,0)

void dummeth_tx_timeout(struct net_device *dev);

#else 

void dummeth_tx_timeout(struct net_device *dev, unsigned int txqueue);

#endif 

 
void dummeth_setup(struct net_device *dev);


 
#endif