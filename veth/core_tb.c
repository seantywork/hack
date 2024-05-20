#include "veth2.h"



const struct veth_q_stat_desc veth_rq_stats_desc[VETH_RQ_STATS_LEN] = {
	{ "xdp_packets",	VETH_RQ_STAT(xdp_packets) },
	{ "xdp_bytes",		VETH_RQ_STAT(xdp_bytes) },
	{ "drops",		VETH_RQ_STAT(rx_drops) },
	{ "xdp_redirect",	VETH_RQ_STAT(xdp_redirect) },
	{ "xdp_drops",		VETH_RQ_STAT(xdp_drops) },
	{ "xdp_tx",		VETH_RQ_STAT(xdp_tx) },
	{ "xdp_tx_errors",	VETH_RQ_STAT(xdp_tx_err) },
};



const struct veth_q_stat_desc veth_tq_stats_desc[VETH_TQ_STATS_LEN] = {
	{ "xdp_xmit",		VETH_RQ_STAT(peer_tq_xdp_xmit) },
	{ "xdp_xmit_errors",	VETH_RQ_STAT(peer_tq_xdp_xmit_err) },
};



struct eth_gstring_t ethtool_stats_keys[ETHTOOL_STATS_KEYLEN] = {
	{ "peer_ifindex" },
};



const struct ethtool_ops veth_ethtool_ops = {
	.get_drvinfo		= veth_get_drvinfo,
	.get_link		= ethtool_op_get_link,
	.get_strings		= veth_get_strings,
	.get_sset_count		= veth_get_sset_count,
	.get_ethtool_stats	= veth_get_ethtool_stats,
	.get_link_ksettings	= veth_get_link_ksettings,
	.get_ts_info		= ethtool_op_get_ts_info,
	.get_channels		= veth_get_channels,
	.set_channels		= veth_set_channels,
};


const struct net_device_ops veth_netdev_ops = {
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
};

const struct xdp_metadata_ops veth_xdp_metadata_ops = {
	.xmo_rx_timestamp		= veth_xdp_rx_timestamp,
	.xmo_rx_hash			= veth_xdp_rx_hash,
};



const struct nla_policy veth_policy[VETH_INFO_MAX + 1] = {
	[VETH_INFO_PEER]	= { .len = sizeof(struct ifinfomsg) },
};


struct rtnl_link_ops veth_link_ops = {
	.kind		= DRV_NAME,
	.priv_size	= sizeof(struct veth_priv),
	.setup		= veth_setup,
	.validate	= veth_validate,
	.newlink	= veth_newlink,
	.dellink	= veth_dellink,
	.policy		= veth_policy,
	.maxtype	= VETH_INFO_MAX,
	.get_link_net	= veth_get_link_net,
	.get_num_tx_queues	= veth_get_num_queues,
	.get_num_rx_queues	= veth_get_num_queues,
};
