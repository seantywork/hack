
#include "core.h"



struct net_device *dummyx_devs[DRV_COUNT];



int lockup = 0;
module_param(lockup, int, 0);

int timeout = DUMMYX_TIMEOUT;
module_param(timeout, int, 0);

/*
 * Do we run in NAPI mode?
 */
int use_napi = 1; 
module_param(use_napi, int, 0);

int pool_size = 8;
module_param(pool_size, int, 0);


void (*dummyx_interrupt)(int, void *, struct pt_regs *);


void dummyx_setup_pool(struct net_device *dev){

	struct dummyx_priv *priv = netdev_priv(dev);
	int i;
	struct dummyx_packet *pkt;

	priv->ppool = NULL;
	for (i = 0; i < pool_size; i++) {
		pkt = kmalloc (sizeof (struct dummyx_packet), GFP_KERNEL);
		if (pkt == NULL) {
			printk (KERN_INFO "out of memory allocating packet pool\n");
			return;
		}
		pkt->dev = dev;
		pkt->next = priv->ppool;
		priv->ppool = pkt;
	}


}


void dummyx_teardown_pool(struct net_device *dev){

	struct dummyx_priv *priv = netdev_priv(dev);
	struct dummyx_packet *pkt;
    
	while ((pkt = priv->ppool)) {
		priv->ppool = pkt->next;
		kfree (pkt);
		/* FIXME - in-flight packets ? */
	}
}    


struct dummyx_packet *dummyx_get_tx_buffer(struct net_device *dev){

	struct dummyx_priv *priv = netdev_priv(dev);
	unsigned long flags;
	struct dummyx_packet *pkt;
    
	spin_lock_irqsave(&priv->lock, flags);
	pkt = priv->ppool;
	if(!pkt) {
		// PDEBUG("Out of Pool\n");
        printk (KERN_INFO "out of pool\n");
		return pkt;
	}
	priv->ppool = pkt->next;
	if (priv->ppool == NULL) {
		printk (KERN_INFO "pool empty\n");
		netif_stop_queue(dev);
	}
	spin_unlock_irqrestore(&priv->lock, flags);
	return pkt;


}

void dummyx_release_buffer(struct dummyx_packet *pkt){

	unsigned long flags;
	struct dummyx_priv *priv = netdev_priv(pkt->dev);
	
	spin_lock_irqsave(&priv->lock, flags);
	pkt->next = priv->ppool;
	priv->ppool = pkt;
	spin_unlock_irqrestore(&priv->lock, flags);
	if (netif_queue_stopped(pkt->dev) && pkt->next == NULL){

        netif_wake_queue(pkt->dev);
    }


}

void dummyx_enqueue_buf(struct net_device *dev, struct dummyx_packet *pkt){

	unsigned long flags;
	struct dummyx_priv *priv = netdev_priv(dev);

	spin_lock_irqsave(&priv->lock, flags);
	pkt->next = priv->rx_queue;  /* FIXME - misorders packets */
	priv->rx_queue = pkt;
	spin_unlock_irqrestore(&priv->lock, flags);


}



struct dummyx_packet *dummyx_dequeue_buf(struct net_device *dev){

	struct dummyx_priv *priv = netdev_priv(dev);
	struct dummyx_packet *pkt;
	unsigned long flags;

	spin_lock_irqsave(&priv->lock, flags);
	pkt = priv->rx_queue;
	if (pkt != NULL){
        priv->rx_queue = pkt->next;
    }
	spin_unlock_irqrestore(&priv->lock, flags);
	return pkt;

}

void dummyx_rx_ints(struct net_device *dev, int enable){

	struct dummyx_priv *priv = netdev_priv(dev);
	priv->rx_int_enabled = enable;
}


void dummyx_rx(struct net_device *dev, struct dummyx_packet *pkt){

    printk(KERN_INFO "dummyx rx\n");
	
    struct sk_buff *skb;
	struct dummyx_priv *priv = netdev_priv(dev);

	skb = dev_alloc_skb(pkt->datalen + 2);
	if (!skb) {
		if (printk_ratelimit()){
            printk(KERN_INFO "dummyx rx: low on mem - packet dropped\n");
        }
		priv->stats.rx_dropped++;
		goto out;
	}
	skb_reserve(skb, 2); 
	memcpy(skb_put(skb, pkt->datalen), pkt->data, pkt->datalen);

	skb->dev = dev;
	skb->protocol = eth_type_trans(skb, dev);
	skb->ip_summed = CHECKSUM_UNNECESSARY; 
	priv->stats.rx_packets++;
	priv->stats.rx_bytes += pkt->datalen;
	netif_rx(skb);

    printk(KERN_INFO "dummyx rx end\n");
  out:
    printk(KERN_INFO "dummyx rx out\n");
	return;

}

int dummyx_poll(struct napi_struct *napi, int budget){


	int npackets = 0;
	struct sk_buff *skb;
	struct dummyx_priv *priv = container_of(napi, struct dummyx_priv, napi);
	struct net_device *dev = priv->dev;
	struct dummyx_packet *pkt;
    
    printk(KERN_INFO "polling\n");

	while (npackets < budget && priv->rx_queue) {
		pkt = dummyx_dequeue_buf(dev);
		skb = dev_alloc_skb(pkt->datalen + 2);
		if (! skb) {
			if (printk_ratelimit()){
                printk(KERN_INFO "dummyx: packet dropped\n");
            }
			priv->stats.rx_dropped++;
			npackets++;
			dummyx_release_buffer(pkt);
			continue;
		}
		skb_reserve(skb, 2);  
		memcpy(skb_put(skb, pkt->datalen), pkt->data, pkt->datalen);
		skb->dev = dev;
		skb->protocol = eth_type_trans(skb, dev);
		skb->ip_summed = CHECKSUM_UNNECESSARY;
		netif_receive_skb(skb);
		
		npackets++;
		priv->stats.rx_packets++;
		priv->stats.rx_bytes += pkt->datalen;
		dummyx_release_buffer(pkt);
	}

    printk(KERN_INFO "polling done\n");

	if (npackets < budget) {
        printk(KERN_INFO "npackets smaller than budget\n");
		unsigned long flags;
		spin_lock_irqsave(&priv->lock, flags);
		if (napi_complete_done(napi, npackets)){
            dummyx_rx_ints(dev, 1);
        }
		spin_unlock_irqrestore(&priv->lock, flags);
	}

    printk(KERN_INFO "polling end\n");

	return npackets;

}

void dummyx_regular_interrupt(int irq, void *dev_id, struct pt_regs *regs){

	int statusword;
	struct dummyx_priv *priv;
	struct dummyx_packet *pkt = NULL;

	struct net_device *dev = (struct net_device *)dev_id;


	if (!dev){
		return;
    }

	priv = netdev_priv(dev);
	spin_lock(&priv->lock);

	/* retrieve statusword: real netdevices use I/O instructions */
	statusword = priv->status;
	priv->status = 0;
	if (statusword & DUMMYX_RX_INTR) {
		
		pkt = priv->rx_queue;
		if (pkt) {
			priv->rx_queue = pkt->next;
			dummyx_rx(dev, pkt);
		}
	}
	if (statusword & DUMMYX_TX_INTR) {
		
		priv->stats.tx_packets++;
		priv->stats.tx_bytes += priv->tx_packetlen;
		dev_kfree_skb(priv->skb);
	}

	spin_unlock(&priv->lock);
	if (pkt) {
        dummyx_release_buffer(pkt); /* Do this outside the lock! */
    }
	return;


}

void dummyx_napi_interrupt(int irq, void *dev_id, struct pt_regs *regs){

    printk(KERN_INFO "napi interrupt\n");

	int statusword;
	struct dummyx_priv *priv;


	struct net_device *dev = (struct net_device *)dev_id;

	if (!dev){
        printk(KERN_INFO "invalid dev\n");
		return;
    }

	priv = netdev_priv(dev);
	spin_lock(&priv->lock);

	/* retrieve statusword: real netdevices use I/O instructions */
	statusword = priv->status;
	priv->status = 0;
	if (statusword & DUMMYX_RX_INTR) {
        printk(KERN_INFO "napi receive\n");
		dummyx_rx_ints(dev, 0);  
		napi_schedule(&priv->napi);
	}
	if (statusword & DUMMYX_TX_INTR) {
        printk(KERN_INFO "napi transmit\n");
		priv->stats.tx_packets++;
		priv->stats.tx_bytes += priv->tx_packetlen;
		if(priv->skb) {
			dev_kfree_skb(priv->skb);
			priv->skb = 0;
		}
	}

    printk(KERN_INFO "napi interrupt end\n");

	spin_unlock(&priv->lock);
	return;
}

void dummyx_hw_tx(char *buf, int len, struct net_device *dev){


    printk(KERN_INFO "entered hw tx\n");
	/*
	 * This function deals with hw details. This interface loops
	 * back the packet to the other dummyx interface (if any).
	 * In other words, this function implements the dummyx behaviour,
	 * while all other procedures are rather device-independent
	 */
	struct iphdr *ih;
	struct net_device *dest;
	struct dummyx_priv *priv;
	u32 *saddr, *daddr;
	struct dummyx_packet *tx_buffer;
    
	if (len < sizeof(struct ethhdr) + sizeof(struct iphdr)) {
		printk("dummyx: packet too short (%i octets)\n",
				len);
		return;
	}

    
    // enable this conditional to look at the data

    int i;
    printk(KERN_INFO "len is %i\n",len);
    //for (i=14 ; i<len; i++)
    //    printk(" %02x",buf[i]&0xff);
    //printk("\n");



	/*
	 * Ethhdr is 14 bytes, but the kernel arranges for iphdr
	 * to be aligned (i.e., ethhdr is unaligned)
	 */
	ih = (struct iphdr *)(buf+sizeof(struct ethhdr));
	saddr = &ih->saddr;
	daddr = &ih->daddr;

	((u8 *)saddr)[2] ^= 1; /* change the third octet (class C) */
	((u8 *)daddr)[2] ^= 1;

	ih->check = 0;         /* and rebuild the checksum (ip needs it) */
	ih->check = ip_fast_csum((unsigned char *)ih,ih->ihl);

    
	if (dev == dummyx_devs[0])
		printk(KERN_INFO "%08x:%05i --> %08x:%05i\n",
				ntohl(ih->saddr),ntohs(((struct tcphdr *)(ih+1))->source),
				ntohl(ih->daddr),ntohs(((struct tcphdr *)(ih+1))->dest));
	else
		printk(KERN_INFO "%08x:%05i <-- %08x:%05i\n",
				ntohl(ih->daddr),ntohs(((struct tcphdr *)(ih+1))->dest),
				ntohl(ih->saddr),ntohs(((struct tcphdr *)(ih+1))->source));

    

	/*
	 * Ok, now the packet is ready for transmission: first simulate a
	 * receive interrupt on the twin device, then  a
	 * transmission-done on the transmitting device
	 */
	dest = dummyx_devs[dev == dummyx_devs[0] ? 1 : 0];
	priv = netdev_priv(dest);
	tx_buffer = dummyx_get_tx_buffer(dev);

	if(!tx_buffer) {
		printk(KERN_INFO "out of tx buffer, len is %i\n",len);
		return;
	}

	tx_buffer->datalen = len;
	memcpy(tx_buffer->data, buf, len);
	dummyx_enqueue_buf(dest, tx_buffer);
	if (priv->rx_int_enabled) {
		priv->status |= DUMMYX_RX_INTR;
		dummyx_interrupt(0, dest, NULL);
	}

	priv = netdev_priv(dev);
	priv->tx_packetlen = len;
	priv->tx_packetdata = buf;
	priv->status |= DUMMYX_TX_INTR;
	if (lockup && ((priv->stats.tx_packets + 1) % lockup) == 0) {
        	/* Simulate a dropped transmit interrupt */
		netif_stop_queue(dev);
		printk(KERN_INFO "simulate lockup at %ld, txp %ld\n", jiffies, (unsigned long) priv->stats.tx_packets);
	}
	else{

        dummyx_interrupt(0, dev, NULL);
    }



}



int dummyx_rebuild_header(struct sk_buff *skb){

	struct ethhdr *eth = (struct ethhdr *) skb->data;
	struct net_device *dev = skb->dev;
    
	memcpy(eth->h_source, dev->dev_addr, dev->addr_len);
	memcpy(eth->h_dest, dev->dev_addr, dev->addr_len);
	eth->h_dest[ETH_ALEN-1]   ^= 0x01;   /* dest is us xor 1 */
	return 0;

}




int dummyx_header(struct sk_buff *skb, struct net_device *dev,
                unsigned short type, const void *daddr, const void *saddr,
                unsigned len){

	struct ethhdr *eth = (struct ethhdr *)skb_push(skb,ETH_HLEN);

	eth->h_proto = htons(type);
	memcpy(eth->h_source, saddr ? saddr : dev->dev_addr, dev->addr_len);
	memcpy(eth->h_dest,   daddr ? daddr : dev->dev_addr, dev->addr_len);
	eth->h_dest[ETH_ALEN-1]   ^= 0x01;   /* dest is us xor 1 */
	return (dev->hard_header_len);

}


int dummyx_change_mtu(struct net_device *dev, int new_mtu){

	unsigned long flags;
	struct dummyx_priv *priv = netdev_priv(dev);
	spinlock_t *lock = &priv->lock;
    
	if ((new_mtu < 68) || (new_mtu > 1500)){

        return -EINVAL;
    }

	spin_lock_irqsave(lock, flags);
	dev->mtu = new_mtu;
	spin_unlock_irqrestore(lock, flags);
	return 0; 

}

int dummyx_open(struct net_device *dev){

	memcpy(dev->dev_addr, "\0DUMX0", ETH_ALEN);
	if (dev == dummyx_devs[1]){
        dev->dev_addr[ETH_ALEN-1]++; 
    }

	if (use_napi) {
		struct dummyx_priv *priv = netdev_priv(dev);
		napi_enable(&priv->napi);
	}
	netif_start_queue(dev);

    printk(KERN_INFO "started dummyx\n");

	return 0;
}

int dummyx_stop(struct net_device *dev){

	netif_stop_queue(dev);
    if (use_napi) {
        struct dummyx_priv *priv = netdev_priv(dev);
        napi_disable(&priv->napi);
    }
	return 0;

    printk(KERN_INFO "stopped dummyx\n");
}


int dummyx_set_config(struct net_device *dev, struct ifmap *map){

	if (dev->flags & IFF_UP){
        return -EBUSY;
    }

	if (map->base_addr != dev->base_addr) {
		printk(KERN_INFO "dummyx: can't change I/O address\n");
		return -EOPNOTSUPP;
	}

	if (map->irq != dev->irq) {

		dev->irq = map->irq;

	}

	return 0;
}

netdev_tx_t dummyx_xmit(struct sk_buff *skb, struct net_device *dev){

    printk(KERN_INFO "entered xmit\n");

	int len;
	char *data, shortpkt[ETH_ZLEN];
	struct dummyx_priv *priv = netdev_priv(dev);

	data = skb->data;
	len = skb->len;
	if (len < ETH_ZLEN) {
		memset(shortpkt, 0, ETH_ZLEN);
		memcpy(shortpkt, skb->data, skb->len);
		len = ETH_ZLEN;
		data = shortpkt;
	}
	netif_trans_update(dev);

	priv->skb = skb;

	dummyx_hw_tx(data, len, dev);


    printk("exiting xmit\n");

	return 0;



}

int dummyx_do_ioctl(struct net_device *dev, struct ifreq *ifr, int cmd){


    printk(KERN_INFO "dummyx ioctl\n");
	return 0;
}

struct net_device_stats* dummyx_get_stats(struct net_device *dev){

	struct dummyx_priv *priv = netdev_priv(dev);
	return &priv->stats;

}


#if LINUX_VERSION_CODE < KERNEL_VERSION(5,6,0)

void dummyx_tx_timeout(struct net_device *dev)

#else 

void dummyx_tx_timeout(struct net_device *dev, unsigned int txqueue)

#endif 

{
	struct dummyx_priv *priv = netdev_priv(dev);
    struct netdev_queue *txq = netdev_get_tx_queue(dev, 0);

	printk(KERN_INFO "transmit timeout at %ld, latency %ld\n", jiffies,
			jiffies - txq->trans_start);
    
	priv->status |= DUMMYX_TX_INTR;
	dummyx_interrupt(0, dev, NULL);
	priv->stats.tx_errors++;

	spin_lock(&priv->lock);
	dummyx_teardown_pool(dev);
	dummyx_setup_pool(dev);
	spin_unlock(&priv->lock);

	netif_wake_queue(dev);
	return;
}

const struct header_ops dummyx_header_ops = {
    .create = dummyx_header
};



const struct net_device_ops dummyx_netdev_ops = {
	.ndo_open            = dummyx_open,
	.ndo_stop            = dummyx_stop,
	.ndo_start_xmit      = dummyx_xmit,
	.ndo_do_ioctl        = dummyx_do_ioctl,
	.ndo_set_config      = dummyx_set_config,
	.ndo_get_stats       = dummyx_get_stats,
	.ndo_change_mtu      = dummyx_change_mtu,
	.ndo_tx_timeout      = dummyx_tx_timeout,
};




void dummyx_setup(struct net_device *dev){

	struct dummyx_priv *priv;

	ether_setup(dev); 
	dev->watchdog_timeo = timeout;
	dev->netdev_ops = &dummyx_netdev_ops;
	dev->header_ops = &dummyx_header_ops;
	dev->flags           |= IFF_NOARP;
	dev->features        |= NETIF_F_HW_CSUM;

	priv = netdev_priv(dev);
	memset(priv, 0, sizeof(struct dummyx_priv));
	if (use_napi) {
		netif_napi_add(dev, &priv->napi, dummyx_poll,2);
	}
	spin_lock_init(&priv->lock);
	priv->dev = dev;

	dummyx_rx_ints(dev, 1);	
	dummyx_setup_pool(dev);
}



static int __init dummyx_init_module(void){

	int err;

    dummyx_interrupt = use_napi ? dummyx_napi_interrupt : dummyx_regular_interrupt;

	dummyx_devs[0] = alloc_netdev(sizeof(struct dummyx_priv), "dummyx%d", NET_NAME_ENUM, dummyx_setup);
	if (!dummyx_devs[0]){
        return -ENOMEM;
    }

	dummyx_devs[1] = alloc_netdev(sizeof(struct dummyx_priv), "dummyx%d", NET_NAME_ENUM, dummyx_setup);

	if (!dummyx_devs[1]){
        return -ENOMEM;
    }

	err = register_netdevice(dummyx_devs[0]);
	if (err < 0) {
        goto err1;
    }

    err = register_netdevice(dummyx_devs[1]);

    if(err < 0) {

        goto err2;
    }


	return 0;

err1:

	free_netdev(dummyx_devs[0]);
	return err;

err2:
	free_netdev(dummyx_devs[0]);
    free_netdev(dummyx_devs[1]);
	return err; 

}



static void __exit dummyx_cleanup_module(void)
{
	int i;
    
	for (i = 0; i < DRV_COUNT; i++) {
		if (dummyx_devs[i]) {
			unregister_netdev(dummyx_devs[i]);
			dummyx_teardown_pool(dummyx_devs[i]);
			free_netdev(dummyx_devs[i]);
		}
	}
	return;
}




module_init(dummyx_init_module);
module_exit(dummyx_cleanup_module);
MODULE_LICENSE("GPL");
