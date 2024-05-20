#ifndef _NET_VETH2_H_
#define _NET_VETH2_H_



#include <linux/netdevice.h>
#include <linux/slab.h>
#include <linux/ethtool.h>
#include <linux/etherdevice.h>
#include <linux/u64_stats_sync.h>

#include <net/rtnetlink.h>
#include <net/dst.h>
#include <net/xfrm.h>
#include <net/xdp.h>
#include <linux/veth.h>
#include <linux/module.h>
#include <linux/bpf.h>
#include <linux/filter.h>
#include <linux/ptr_ring.h>
#include <linux/bpf_trace.h>
#include <linux/net_tstamp.h>
#include <net/page_pool.h>


#define DRV_NAME	"veth2"
#define DRV_VERSION	"1.0"

#define VETH_XDP_FLAG		BIT(0)
#define VETH_RING_SIZE		256
#define VETH_XDP_HEADROOM	(XDP_PACKET_HEADROOM + NET_IP_ALIGN)

#define VETH_XDP_TX_BULK_SIZE	16
#define VETH_XDP_BATCH		16

struct veth_stats {
	u64	rx_drops;
	/* xdp */
	u64	xdp_packets;
	u64	xdp_bytes;
	u64	xdp_redirect;
	u64	xdp_drops;
	u64	xdp_tx;
	u64	xdp_tx_err;
	u64	peer_tq_xdp_xmit;
	u64	peer_tq_xdp_xmit_err;
};

struct veth_rq_stats {
	struct veth_stats	vs;
	struct u64_stats_sync	syncp;
};

struct veth_rq {
	struct napi_struct	xdp_napi;
	struct napi_struct __rcu *napi; /* points to xdp_napi when the latter is initialized */
	struct net_device	*dev;
	struct bpf_prog __rcu	*xdp_prog;
	struct xdp_mem_info	xdp_mem;
	struct veth_rq_stats	stats;
	bool			rx_notify_masked;
	struct ptr_ring		xdp_ring;
	struct xdp_rxq_info	xdp_rxq;
	struct page_pool	*page_pool;
};

struct veth_priv {
	struct net_device __rcu	*peer;
	atomic64_t		dropped;
	struct bpf_prog		*_xdp_prog;
	struct veth_rq		*rq;
	unsigned int		requested_headroom;
};

struct veth_xdp_tx_bq {
	struct xdp_frame *q[VETH_XDP_TX_BULK_SIZE];
	unsigned int count;
};

/*
 * ethtool interface
 */

struct veth_q_stat_desc {
	char	desc[ETH_GSTRING_LEN];
	size_t	offset;
};

struct veth_xdp_buff {
	struct xdp_buff xdp;
	struct sk_buff *skb;
};

struct eth_gstring_t {
    char string[ETH_GSTRING_LEN];
};

#define VETH_FEATURES (NETIF_F_SG | NETIF_F_FRAGLIST | NETIF_F_HW_CSUM | \
		       NETIF_F_RXCSUM | NETIF_F_SCTP_CRC | NETIF_F_HIGHDMA | \
		       NETIF_F_GSO_SOFTWARE | NETIF_F_GSO_ENCAP_ALL | \
		       NETIF_F_HW_VLAN_CTAG_TX | NETIF_F_HW_VLAN_CTAG_RX | \
		       NETIF_F_HW_VLAN_STAG_TX | NETIF_F_HW_VLAN_STAG_RX )


#define VETH_RQ_STAT(m)	offsetof(struct veth_stats, m)

#define VETH_RQ_STATS_LEN 7

#define VETH_TQ_STATS_LEN 2

#define ETHTOOL_STATS_KEYLEN 1


extern const struct veth_q_stat_desc veth_rq_stats_desc[VETH_RQ_STATS_LEN];

extern const struct veth_q_stat_desc veth_tq_stats_desc[VETH_TQ_STATS_LEN];

extern struct eth_gstring_t ethtool_stats_keys[ETHTOOL_STATS_KEYLEN];


extern struct net_device *DEV2;
extern struct net_device *PEER2;

/*

	.get_drvinfo		= veth_get_drvinfo,
	.get_link		= ethtool_op_get_link,
	.get_strings		= veth_get_strings,
	.get_sset_count		= veth_get_sset_count,
	.get_ethtool_stats	= veth_get_ethtool_stats,
	.get_link_ksettings	= veth_get_link_ksettings,
	.get_ts_info		= ethtool_op_get_ts_info,
	.get_channels		= veth_get_channels,
	.set_channels		= veth_set_channels,
*/

extern const struct ethtool_ops veth_ethtool_ops;

/*

	.ndo_init            = veth_dev_init,
	.ndo_open            = veth_open,
	.ndo_stop            = veth_close,
	.ndo_start_xmit      = veth_xmit,
	.ndo_get_stats64     = veth_get_stats64,
	.ndo_set_rx_mode     = veth_set_multicast_list,
	.ndo_set_mac_address = eth_mac_addr,
#ifdef CONFIG_NET_POLL_CONTROLLER
	.ndo_poll_controller	= veth_poll_controller,
#endif
	.ndo_get_iflink		= veth_get_iflink,
	.ndo_fix_features	= veth_fix_features,
	.ndo_set_features	= veth_set_features,
	.ndo_features_check	= passthru_features_check,
	.ndo_set_rx_headroom	= veth_set_rx_headroom,
	.ndo_bpf		= veth_xdp,
	.ndo_xdp_xmit		= veth_ndo_xdp_xmit,
	.ndo_get_peer_dev	= veth_peer_dev,


*/

extern const struct net_device_ops veth_netdev_ops;

/*

	.xmo_rx_timestamp		= veth_xdp_rx_timestamp,
	.xmo_rx_hash			= veth_xdp_rx_hash,
*/

extern const struct xdp_metadata_ops veth_xdp_metadata_ops;

extern const struct nla_policy veth_policy[];

extern struct rtnl_link_ops veth_link_ops;


/*************************/
// ethtool

void veth_get_drvinfo(struct net_device *dev, struct ethtool_drvinfo *info);


void veth_get_strings(struct net_device *dev, u32 stringset, u8 *buf);

int veth_get_sset_count(struct net_device *dev, int sset);

void veth_get_ethtool_stats(struct net_device *dev,
		struct ethtool_stats *stats, u64 *data);

int veth_get_link_ksettings(struct net_device *dev,
				   struct ethtool_link_ksettings *cmd);

void veth_get_channels(struct net_device *dev,
			      struct ethtool_channels *channels);

int veth_set_channels(struct net_device *dev,
			     struct ethtool_channels *ch);

/**************************/
// netdev

int veth_dev_init(struct net_device *dev);

int veth_open(struct net_device *dev);

int veth_close(struct net_device *dev);

netdev_tx_t veth_xmit(struct sk_buff *skb, struct net_device *dev);

void veth_get_stats64(struct net_device *dev,
			     struct rtnl_link_stats64 *tot);

void veth_set_multicast_list(struct net_device *dev);

void veth_poll_controller(struct net_device *dev);

int veth_get_iflink(const struct net_device *dev);

netdev_features_t veth_fix_features(struct net_device *dev,
					   netdev_features_t features);


int veth_set_features(struct net_device *dev,
			     netdev_features_t features);

void veth_set_rx_headroom(struct net_device *dev, int new_hr);

int veth_xdp(struct net_device *dev, struct netdev_bpf *xdp);

int veth_ndo_xdp_xmit(struct net_device *dev, int n,
			     struct xdp_frame **frames, u32 flags);

struct net_device *veth_peer_dev(struct net_device *dev);

/**************************/
// xdp_metadata

int veth_xdp_rx_timestamp(const struct xdp_md *ctx, u64 *timestamp);

int veth_xdp_rx_hash(const struct xdp_md *ctx, u32 *hash,
			    enum xdp_rss_hash_type *rss_type);


/*************************/
// ex

void veth_get_page_pool_stats(struct net_device *dev, u64 *data);

bool veth_is_xdp_frame(void *ptr);

struct xdp_frame *veth_ptr_to_xdp(void *ptr);

void *veth_xdp_to_ptr(struct xdp_frame *xdp);

void veth_ptr_free(void *ptr);

void __veth_xdp_flush(struct veth_rq *rq);

int veth_xdp_rx(struct veth_rq *rq, struct sk_buff *skb);

int veth_forward_skb(struct net_device *dev, struct sk_buff *skb,
			    struct veth_rq *rq, bool xdp);

bool veth_skb_is_eligible_for_gro(const struct net_device *dev,
					 const struct net_device *rcv,
					 const struct sk_buff *skb);

u64 veth_stats_tx(struct net_device *dev, u64 *packets, u64 *bytes);

void veth_stats_rx(struct veth_stats *result, struct net_device *dev);

int veth_select_rxq(struct net_device *dev);

int veth_xdp_xmit(struct net_device *dev, int n,
			 struct xdp_frame **frames,
			 u32 flags, bool ndo_xmit);

void veth_xdp_flush_bq(struct veth_rq *rq, struct veth_xdp_tx_bq *bq);

void veth_xdp_flush(struct veth_rq *rq, struct veth_xdp_tx_bq *bq);

int veth_xdp_tx(struct veth_rq *rq, struct xdp_buff *xdp,
		       struct veth_xdp_tx_bq *bq);

struct xdp_frame *veth_xdp_rcv_one(struct veth_rq *rq,
					  struct xdp_frame *frame,
					  struct veth_xdp_tx_bq *bq,
					  struct veth_stats *stats);


void veth_xdp_rcv_bulk_skb(struct veth_rq *rq, void **frames,
				  int n_xdpf, struct veth_xdp_tx_bq *bq,
				  struct veth_stats *stats);

void veth_xdp_get(struct xdp_buff *xdp);

int veth_convert_skb_to_xdp_buff(struct veth_rq *rq,
					struct xdp_buff *xdp,
					struct sk_buff **pskb);

struct sk_buff *veth_xdp_rcv_skb(struct veth_rq *rq,
					struct sk_buff *skb,
					struct veth_xdp_tx_bq *bq,
					struct veth_stats *stats);

int veth_xdp_rcv(struct veth_rq *rq, int budget,
			struct veth_xdp_tx_bq *bq,
			struct veth_stats *stats);

int veth_poll(struct napi_struct *napi, int budget);

int veth_create_page_pool(struct veth_rq *rq);

int __veth_napi_enable_range(struct net_device *dev, int start, int end);

int __veth_napi_enable(struct net_device *dev);

void veth_napi_del_range(struct net_device *dev, int start, int end);

void veth_napi_del(struct net_device *dev);

bool veth_gro_requested(const struct net_device *dev);

int veth_enable_xdp_range(struct net_device *dev, int start, int end,
				 bool napi_already_on);

void veth_disable_xdp_range(struct net_device *dev, int start, int end,
				   bool delete_napi);

int veth_enable_xdp(struct net_device *dev);

void veth_disable_xdp(struct net_device *dev);

int veth_napi_enable_range(struct net_device *dev, int start, int end);

int veth_napi_enable(struct net_device *dev);

void veth_disable_range_safe(struct net_device *dev, int start, int end);

int veth_enable_range_safe(struct net_device *dev, int start, int end);

void veth_set_xdp_features(struct net_device *dev);

int is_valid_veth_mtu(int mtu);

int veth_alloc_queues(struct net_device *dev);

void veth_free_queues(struct net_device *dev);

void veth_dev_free(struct net_device *dev);

int veth_xdp_set(struct net_device *dev, struct bpf_prog *prog,
			struct netlink_ext_ack *extack);

void veth_disable_gro(struct net_device *dev);

int veth_init_queues(struct net_device *dev, struct nlattr *tb[]);


/****************************/
// core


void veth_setup(struct net_device *dev);

int veth_validate(struct nlattr *tb[], struct nlattr *data[],
			 struct netlink_ext_ack *extack);

int veth_newlink(struct net *src_net, struct net_device *dev,
			struct nlattr *tb[], struct nlattr *data[],
			struct netlink_ext_ack *extack);

void veth_dellink(struct net_device *dev, struct list_head *head);

struct net *veth_get_link_net(const struct net_device *dev);

unsigned int veth_get_num_queues(void);




#endif