#ifndef _LOCOBOX_H_
#define _LOCOBOX_H_



#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/ethtool.h>
#include <linux/init.h>
#include <linux/moduleparam.h>
#include <linux/rtnetlink.h>
#include <linux/net_tstamp.h>
#include <net/rtnetlink.h>
#include <linux/u64_stats_sync.h>

#define DRV_NAME	"locobox"

extern struct net_device* dev_locobox;

extern int numlocobox;

extern const struct net_device_ops locobox_netdev_ops;

extern const struct ethtool_ops locobox_ethtool_ops;

void set_multicast_list(struct net_device *dev);

void locobox_get_stats64(struct net_device *dev,
			      struct rtnl_link_stats64 *stats);

netdev_tx_t locobox_xmit(struct sk_buff *skb, struct net_device *dev);

int locobox_dev_init(struct net_device *dev);

void locobox_dev_uninit(struct net_device *dev);

int locobox_change_carrier(struct net_device *dev, bool new_carrier);

void locobox_setup(struct net_device *dev);

#endif