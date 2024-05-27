#include "core.h"


int use_napi = 1; 
//module_param(use_napi, int, 0);

int timeout = DUMMETH_TIMEOUT;
//module_param(timeout, int, 0);

int pool_size = 8;

struct net_device *device;
struct net_device_stats *stats;
struct napi_struct dummeth_napi_struct;


void (*dummeth_interrupt)(int, void *, struct pt_regs *);

int eth_header_create(struct sk_buff *skb, struct net_device *dev,
                unsigned short type, const void *daddr, const void *saddr,
                unsigned len){
 
    struct ethhdr *eth = (struct ethhdr *)skb_push(skb,ETH_HLEN);
 
    eth->h_proto = htons(type);
    memcpy(eth->h_source, saddr ? saddr : dev->dev_addr, dev->addr_len);
    memcpy(eth->h_dest, daddr ? daddr : dev->dev_addr, dev->addr_len);
    //eth->h_dest[ETH_ALEN-1] ^= 0x01; /* dest is us xor 1 */
    return (dev->hard_header_len);
}





void dummeth_setup_pool(struct net_device *dev){

	struct eth_priv *priv = netdev_priv(dev);
	int i;
	struct eth_packet *pkt;

	priv->ppool = NULL;
	for (i = 0; i < pool_size; i++) {
		pkt = kmalloc (sizeof (struct eth_packet), GFP_KERNEL);
		if (pkt == NULL) {
			printk (KERN_INFO "out of memory allocating packet pool\n");
			return;
		}
		pkt->dev = dev;
		pkt->next = priv->ppool;
		priv->ppool = pkt;
	}


}


void dummeth_teardown_pool(struct net_device *dev){

	struct eth_priv *priv = netdev_priv(dev);
	struct eth_packet *pkt;
    
	while ((pkt = priv->ppool)) {
		priv->ppool = pkt->next;
		kfree (pkt);
		/* FIXME - in-flight packets ? */
	}
}    

struct eth_packet *eth_dequeue_buf(struct net_device *dev) {
 
    struct eth_priv *priv = netdev_priv(dev);
    struct eth_packet *pkt;
    unsigned long flags;
 
    spin_lock_irqsave(&priv->lock, flags);
    pkt = priv->rx_queue;
    if (pkt != NULL)
        priv->rx_queue = pkt->next;
    spin_unlock_irqrestore(&priv->lock, flags);
    return pkt;
}

void eth_release_buffer(struct eth_packet *pkt) {
    unsigned long flags;
    struct eth_priv *priv = netdev_priv(pkt->dev);
 
    spin_lock_irqsave(&priv->lock, flags);
    pkt->next = priv->ppool;
    priv->ppool = pkt;
    spin_unlock_irqrestore(&priv->lock, flags);
    if(netif_queue_stopped(pkt->dev) && pkt->next == NULL)
    netif_wake_queue(pkt->dev);
}

void dummeth_rx(struct net_device *dev, struct eth_packet *pkt) {
    struct sk_buff *skb;
    struct eth_priv * priv = netdev_priv(dev);
 
    /*
     * The packet has been retrieved from the transmission
     * medium. Build ans skb around it, so upper layers can handle it
     */
    skb = dev_alloc_skb(pkt->datalen + 2); //alocating the buffer for the packet
 
    /* Checking if the packet allocation process went wrong */
    if (!skb) {
        if (printk_ratelimit())
            printk(KERN_NOTICE "dummeth rx: low on mem - packet dropped\n");
        priv->stats.rx_dropped++;
        goto out;
    }
    memcpy(skb_put(skb, pkt->datalen), pkt->data, pkt->datalen);   //No problems, so we can copy the packet to the buffer.
 
    /* Write metadata, and then pass to the receive level */
    skb->dev = dev;
    skb->protocol = eth_type_trans(skb, dev);
    skb->ip_summed = CHECKSUM_UNNECESSARY;   /* don't check it */
    priv->stats.rx_packets++;
    priv->stats.rx_bytes += pkt->datalen;
    netif_rx(skb);
    out:
        return;
}

void dummeth_rx_ints(struct net_device *dev, int enable) {
    struct eth_priv *priv = netdev_priv(dev);
    priv->rx_int_enabled = enable;
}

int dummeth_napi_poll(struct napi_struct *napi, int budget) {

    printk(KERN_INFO "dummeth: napi poll\n");
    int npackets = 0;
    struct sk_buff *skb;
    struct eth_priv *priv = container_of(napi, struct eth_priv, napi);
    struct net_device *dev = priv->dev;
    struct eth_packet *pkt;
 
    while (npackets < budget && priv->rx_queue) {
        pkt = eth_dequeue_buf(dev);
        skb = dev_alloc_skb(pkt->datalen + 2);
        if (!skb) {
            if (printk_ratelimit())
                printk(KERN_NOTICE "dummeth: packet dropped\n");
            priv->stats.rx_dropped++;
            eth_release_buffer(pkt);
            continue;
        }
        skb_reserve(skb, 2);  //align IP on 16B boundary
        memcpy(skb_put(skb, pkt->datalen), pkt->data, pkt->datalen);
        skb->dev = dev;
        skb->protocol = eth_type_trans(skb, dev);
        skb->ip_summed = CHECKSUM_UNNECESSARY; // don't check it
        netif_receive_skb(skb);
        /* Maintain stats */
        npackets++;
        priv->stats.rx_packets++;
        priv->stats.rx_bytes += pkt->datalen;
        eth_release_buffer(pkt);
    }

    printk(KERN_INFO "dummeth: napi poll: end\n");
    /* If we processed all packets, we're done; tell the kernel and re-enable interruptions */
    /* If budget not fully consumed, exit the polling mode */
    if (npackets < budget) {
        printk(KERN_INFO "dummeth: napi interrupt: complete\n");
        napi_complete(napi);
        /* Enabling the normal interrupt */
        dummeth_rx_ints(dev, 1);
    }
    printk(KERN_INFO "dummeth: napi interrupt success\n");

    return npackets;
}
 
 

 
void dummeth_napi_interrupt(int irq, void *dev_id, struct pt_regs *regs) {

    printk(KERN_INFO "dummeth: napi insterrupt\n");

    int statusword;
    struct eth_priv *priv;
    struct eth_packet *pkt = NULL;
 
    /*
     * As usual, check the "device" pointer to be sure it is
     * really interrupting.
     * Then assign "struct device *dev".
     */
     struct net_device *dev = (struct net_device *)dev_id;
     /* ... and check with hw if it's really ours */
 
     /* paranoid */
     if(!dev){
        return;
     }
    /* Lock the device */
    priv = netdev_priv(dev);
    spin_lock(&priv->lock);
 
    /* retrieve statusword: real netdevices use I/O instructions */
    statusword = priv->status;
    priv->status = 0;

    if(statusword & ETH_RX_INTR) {
        printk(KERN_INFO "dummeth: napi interrupt: rx\n");
        
        /* This will disinable any further "packet available"
         * interrupts and tells networking subsystem to poll
         * the driver shortly to pick up all available packets.
         */
        dummeth_rx_ints(dev, 0);
        if (napi_schedule_prep(&priv->napi)) {
            /* Disinable reception interrupts */
                __napi_schedule(&priv->napi);
        }
         
    }
    if (statusword & ETH_TX_INTR) {
        printk(KERN_INFO "dummeth: napi interrupt: tx\n");
        /* a transmission is over: free the skb */
        priv->stats.tx_packets++;
        priv->stats.tx_bytes += priv->tx_packetlen;
        dev_kfree_skb(priv->skb);
    }
 
    /* Unlock the device and we are done */
    spin_unlock(&priv->lock);
    if (pkt){
        eth_release_buffer(pkt); /* Do this outside the lock! */
    }
    printk(KERN_INFO "dummeth: napi interrupt success\n");
    return;
}   
 

int dummeth_open(struct net_device *dev){
    printk(KERN_INFO "dummeth: open\n");

	if (use_napi) {
		struct eth_priv *priv = netdev_priv(dev);
		napi_enable(&priv->napi);
	}

    netif_start_queue(dev);

    printk(KERN_INFO "dummeth: open success\n");
    return 0;
}

int dummeth_close(struct net_device *dev){

    printk(KERN_INFO "dummeth: close\n");
 	if (use_napi) {
		struct eth_priv *priv = netdev_priv(dev);
		napi_disable(&priv->napi);
	}

    netif_stop_queue(dev);

    printk(KERN_INFO "dummeth: close success\n");
    return 0;
}
 

int dummeth_start_xmit(struct sk_buff *skb, struct net_device *dev) {
    int len;
    char *data, shortpkt[ETH_ZLEN];
    struct eth_priv *priv = netdev_priv(dev);
 
    printk(KERN_INFO "dummeth: xmit\n");

    data = skb->data;
    len = skb->len;
    if (len < ETH_ZLEN) {
        memset(shortpkt, 0 , ETH_ZLEN);
        memcpy(shortpkt, skb->data, skb->len);
        len = ETH_ZLEN;
        data = shortpkt;
    }
    
    netif_trans_update(dev);
 
    /* Remember the skb, so we can free it at interrupt time */
    priv->skb = skb;
 
    /* actual deliver of data is device-specific, and not shown here */
    //nf10_hw_tx(data, len, dev);
 
    printk(KERN_INFO "dummeth: xmit success\n");

    return 0; /* Our simple device can not fail */
}


int dummeth_do_ioctl(struct net_device *dev, struct ifreq *ifr, int cmd) {
    pr_info("dummeth_do_ioctl(%s)\n", dev->name);
    return -1;
}
 

struct net_device_stats *dummeth_get_stats(struct net_device *dev){

    //pr_info("dummeth_get_stats(%s)\n", dev->name);
	struct eth_priv *priv = netdev_priv(dev);
	return &priv->stats;

}

int dummeth_config(struct net_device *dev, struct ifmap *map){

    pr_info("dummeth_config(%s)\n", dev->name);
    if (dev->flags & IFF_UP) {
        return -EBUSY;
    }
	if (map->base_addr != dev->base_addr) {
		printk(KERN_INFO "dummeth: can't change I/O address\n");
		return -EOPNOTSUPP;
	}


	if (map->irq != dev->irq) {

		dev->irq = map->irq;

	}
    return 0;
}
 


int dummeth_change_mtu(struct net_device *dev, int new_mtu){

    unsigned long flags = 0;
    struct eth_priv *priv = netdev_priv(dev);
    spinlock_t *lock = &priv->lock;
 
    pr_info("dummeth_change_mtu(%s)\n", dev->name);
 
    /* check ranges */
    if ((new_mtu < 68) || (new_mtu > 1500))  //remember to see at the hardware documentation the right especification
        return -EINVAL;
 
    /*
     * Do anything you need, and accept the value
     */
    spin_unlock_irqrestore(lock, flags);
    dev->mtu = new_mtu;
    spin_unlock_irqrestore(lock, flags);
    printk (KERN_INFO "New mtu: (%d)", dev->mtu);
    return 0; /* Sucess */
}
 


#if LINUX_VERSION_CODE < KERNEL_VERSION(5,6,0)

void dummeth_tx_timeout(struct net_device *dev)

#else 

void dummeth_tx_timeout(struct net_device *dev, unsigned int txqueue)

#endif 
{
    struct eth_priv *priv = netdev_priv(dev);
    struct netdev_queue *txq = netdev_get_tx_queue(dev, 0);
 
    //PDEBUG ("Transmit timeout at %ld, latency %ls'n", jiffies,
    //          jiffies - dev->trans_start);
    printk(KERN_DEBUG "dummeth: transmit timeout at %ld, latency %ld \n", jiffies,
                jiffies - txq->trans_start);
    /* Simulate a transmission interrupt to get things moving */
    priv->status = ETH_TX_INTR;
    dummeth_interrupt(0, dev, NULL);
    priv->stats.tx_errors++;
    netif_wake_queue(dev);
    return;
}



 
struct header_ops ho = {
    .create = eth_header_create,
};

struct net_device_ops ndo = {
    .ndo_open       = dummeth_open,
    .ndo_stop       = dummeth_close,
    .ndo_start_xmit = dummeth_start_xmit,
    .ndo_do_ioctl   = dummeth_do_ioctl,
    .ndo_get_stats  = dummeth_get_stats,
    .ndo_set_config = dummeth_config,
    .ndo_change_mtu = dummeth_change_mtu,
    .ndo_tx_timeout = dummeth_tx_timeout,
    //.ndo_poll_controller = dummeth_napi_struct_poll;
};


void dummeth_setup(struct net_device *dev){

    //pr_info("dummeth_setup(%s)\n", dev->name);
    
    printk(KERN_INFO "dummeth: setup\n");

    struct eth_priv *priv;

    ether_setup(dev);

	dev->watchdog_timeo = timeout;
	dev->netdev_ops = &ndo;
	dev->header_ops = &ho;
	dev->flags           |= IFF_NOARP;
	dev->features        |= NETIF_F_HW_CSUM;

	priv = netdev_priv(dev);
	memset(priv, 0, sizeof(struct eth_priv));
	if (use_napi) {
        printk(KERN_INFO "dummeth: use_napi\n");
		netif_napi_add(dev, &priv->napi, dummeth_napi_poll,16);
	}
	spin_lock_init(&priv->lock);
	priv->dev = dev;

	dummeth_rx_ints(dev, 1);	
    dummeth_setup_pool(dev);

    eth_hw_addr_random(dev);

    printk(KERN_INFO "dummeth: setup success\n");


}


static int __init dummeth_driver_init(void)
{
    int result;
    struct eth_priv *priv;
    printk(KERN_INFO "dummeth: init");

    dummeth_interrupt = dummeth_napi_interrupt;
 
    priv = 0; // TODO: GET priv
 
    /* Add NAPI structure to the device. */
        /* We just use the only netdevice for implementing polling. */
    //netif_napi_add(device, &priv->napi, dummeth_napi_struct_poll, NAPI_POLL_WEIGHT);
 
    /* Allocating the net device. */
    device = alloc_netdev(sizeof(struct eth_priv), "dummeth%d", NET_NAME_ENUM, dummeth_setup);
 
    if(!device){
        return -ENOMEM;
    }

    result = register_netdev(device);

    if(result < 0){
        free_netdev(device);
        return result;

    }

    printk(KERN_INFO "dummeth: init success\n");
    return 0;
}
 
static void __exit dummeth_driver_exit(void)
{
    printk(KERN_INFO "dummeth: exit\n");
    if (device) {
        unregister_netdev(device);
        //netif_napi_del();     //Doesn't neet to use Napi exit because free_netdev() does that.
        printk(KERN_INFO "dummeth: device unregistered\n");
        dummeth_teardown_pool(device);
        free_netdev(device);
        printk(KERN_INFO "dummeth: exit success\n");
    }
    return;
}

 
module_init(dummeth_driver_init);
module_exit(dummeth_driver_exit);
 
MODULE_LICENSE("GPL");