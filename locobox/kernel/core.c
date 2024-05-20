
#include "core.h"

struct net_device* dev_locobox = NULL;

int numlocobox = 1;

/* fake multicast ability */
void set_multicast_list(struct net_device *dev){
}

void locobox_get_stats64(struct net_device *dev,
			      struct rtnl_link_stats64 *stats){

	dev_lstats_read(dev, &stats->tx_packets, &stats->tx_bytes);
}

netdev_tx_t locobox_xmit(struct sk_buff *skb, struct net_device *dev){

	dev_lstats_add(dev, skb->len);

	skb_tx_timestamp(skb);
	dev_kfree_skb(skb);
	return NETDEV_TX_OK;
}

int locobox_dev_init(struct net_device *dev){

	dev->lstats = netdev_alloc_pcpu_stats(struct pcpu_lstats);
	if (!dev->lstats)
		return -ENOMEM;

	return 0;
}

void locobox_dev_uninit(struct net_device *dev){
	free_percpu(dev->lstats);
}

int locobox_change_carrier(struct net_device *dev, bool new_carrier){

	if (new_carrier)
		netif_carrier_on(dev);
	else
		netif_carrier_off(dev);
	return 0;
}

const struct net_device_ops locobox_netdev_ops = {
	.ndo_init		= locobox_dev_init,
	.ndo_uninit		= locobox_dev_uninit,
	.ndo_start_xmit		= locobox_xmit,
	.ndo_validate_addr	= eth_validate_addr,
	.ndo_set_rx_mode	= set_multicast_list,
	.ndo_set_mac_address	= eth_mac_addr,
	.ndo_get_stats64	= locobox_get_stats64,
	.ndo_change_carrier	= locobox_change_carrier,
};

const struct ethtool_ops locobox_ethtool_ops = {
	.get_ts_info		= ethtool_op_get_ts_info,
};

void locobox_setup(struct net_device *dev){

	ether_setup(dev);

	/* Initialize the device structure. */
	dev->netdev_ops = &locobox_netdev_ops;
	dev->ethtool_ops = &locobox_ethtool_ops;
	dev->needs_free_netdev = true;

	/* Fill in device structure with ethernet-generic values. */
	dev->flags |= IFF_NOARP;
	dev->flags &= ~IFF_MULTICAST;
	dev->priv_flags |= IFF_LIVE_ADDR_CHANGE | IFF_NO_QUEUE;
	dev->features	|= NETIF_F_SG | NETIF_F_FRAGLIST;
	dev->features	|= NETIF_F_GSO_SOFTWARE;
	dev->features	|= NETIF_F_HW_CSUM | NETIF_F_HIGHDMA | NETIF_F_LLTX;
	dev->features	|= NETIF_F_GSO_ENCAP_ALL;
	dev->hw_features |= dev->features;
	dev->hw_enc_features |= dev->features;
	eth_hw_addr_random(dev);

	dev->min_mtu = 0;
	dev->max_mtu = 0;
}


/* Number of locobox devices to be set up by this module. */
module_param(numlocobox, int, 0);
MODULE_PARM_DESC(numlocobox, "Number of locobox pseudo devices");

static int __init locobox_init_module(void){

	int err;

	dev_locobox = alloc_netdev(0, "locobox%d", NET_NAME_ENUM, locobox_setup);
	if (!dev_locobox)
		return -ENOMEM;

	err = register_netdevice(dev_locobox);
	if (err < 0)
		goto err;
	return 0;

err:
	free_netdev(dev_locobox);
	return err;
}



static void __exit locobox_cleanup_module(void)
{
	unregister_netdevice(dev_locobox);
}

module_init(locobox_init_module);
module_exit(locobox_cleanup_module);
MODULE_LICENSE("GPL");
