#ifndef _DUMMYX_SINGLE_H_
#define _DUMMYX_SINGLE_H_



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

#define DRV_NAME	"dummyx_single"

extern struct net_device* dev_dummyx;

extern int numdummyx;

extern const struct net_device_ops dummyx_netdev_ops;

extern const struct ethtool_ops dummyx_ethtool_ops;

void set_multicast_list(struct net_device *dev);

void dummyx_get_stats64(struct net_device *dev,
			      struct rtnl_link_stats64 *stats);

netdev_tx_t dummyx_xmit(struct sk_buff *skb, struct net_device *dev);

int dummyx_dev_init(struct net_device *dev);

void dummyx_dev_uninit(struct net_device *dev);

int dummyx_change_carrier(struct net_device *dev, bool new_carrier);

void dummyx_setup(struct net_device *dev);

#endif