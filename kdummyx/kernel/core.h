#ifndef _DUMMYX_H_ 
#define _DUMMYX_H_ 



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

#define DRV_NAME	"dummyx"
#define DRV_COUNT   2


#define DUMMYX_RX_INTR 0x0001
#define DUMMYX_TX_INTR 0x0002

/* Default timeout period */
#define DUMMYX_TIMEOUT 5   /* In jiffies */



struct dummyx_packet {
	struct dummyx_packet *next;
	struct net_device *dev;
	int	datalen;
	u8 data[ETH_DATA_LEN];
};

struct dummyx_priv {
    struct net_device_stats stats;
    int status;
    struct dummyx_packet *ppool;
    struct dummyx_packet *rx_queue; /* List of incoming packets */
    int rx_int_enabled;
    int tx_packetlen;
    u8 *tx_packetdata;
    struct sk_buff *skb;
    spinlock_t lock;
	struct net_device *dev;
	struct napi_struct napi;
};


extern struct net_device *dummyx_devs[DRV_COUNT];


extern const struct net_device_ops dummyx_netdev_ops;

extern const struct header_ops dummyx_header_ops;

extern int lockup;

extern int timeout;

extern int use_napi;

extern int pool_size;

extern void (*dummyx_interrupt)(int, void *, struct pt_regs *);

/* util functions */

void dummyx_setup_pool(struct net_device *dev);

void dummyx_teardown_pool(struct net_device *dev);

struct dummyx_packet *dummyx_get_tx_buffer(struct net_device *dev);

void dummyx_release_buffer(struct dummyx_packet *pkt);

void dummyx_enqueue_buf(struct net_device *dev, struct dummyx_packet *pkt);

struct dummyx_packet *dummyx_dequeue_buf(struct net_device *dev);


/* rx, interrupt functions */

void dummyx_rx_ints(struct net_device *dev, int enable);

void dummyx_rx(struct net_device *dev, struct dummyx_packet *pkt);

int dummyx_poll(struct napi_struct *napi, int budget);

void dummyx_regular_interrupt(int irq, void *dev_id, struct pt_regs *regs);

void dummyx_napi_interrupt(int irq, void *dev_id, struct pt_regs *regs);

/* device specific hw tx functions */

void dummyx_hw_tx(char *buf, int len, struct net_device *dev);


/* header and etc */

int dummyx_rebuild_header(struct sk_buff *skb);

int dummyx_header(struct sk_buff *skb, struct net_device *dev,
                unsigned short type, const void *daddr, const void *saddr,
                unsigned len);

int dummyx_change_mtu(struct net_device *dev, int new_mtu);


/* netdev */

int dummyx_open(struct net_device *dev);

int dummyx_stop(struct net_device *dev);

int dummyx_set_config(struct net_device *dev, struct ifmap *map);

netdev_tx_t dummyx_xmit(struct sk_buff *skb, struct net_device *dev);

int dummyx_do_ioctl(struct net_device *dev, struct ifreq *ifr, int cmd);

//int dummyx_get_stats(struct net_device *dev);

struct net_device_stats* dummyx_get_stats(struct net_device *dev);

#if LINUX_VERSION_CODE < KERNEL_VERSION(5,6,0)

void dummyx_tx_timeout(struct net_device *dev);

#else 

void dummyx_tx_timeout(struct net_device *dev, unsigned int txqueue);

#endif 


/* module entry */

void dummyx_setup(struct net_device *dev);

#endif