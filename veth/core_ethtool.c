#include "veth2.h"


void veth_get_drvinfo(struct net_device *dev, struct ethtool_drvinfo *info){

	strscpy(info->driver, DRV_NAME, sizeof(info->driver));
	strscpy(info->version, DRV_VERSION, sizeof(info->version));

}



void veth_get_strings(struct net_device *dev, u32 stringset, u8 *buf){

	u8 *p = buf;
	int i, j;

	switch(stringset) {
	case ETH_SS_STATS:
		memcpy(p, &ethtool_stats_keys, sizeof(ethtool_stats_keys));
		p += sizeof(ethtool_stats_keys);
		for (i = 0; i < dev->real_num_rx_queues; i++)
			for (j = 0; j < VETH_RQ_STATS_LEN; j++)
				ethtool_sprintf(&p, "rx_queue_%u_%.18s",
						i, veth_rq_stats_desc[j].desc);

		for (i = 0; i < dev->real_num_tx_queues; i++)
			for (j = 0; j < VETH_TQ_STATS_LEN; j++)
				ethtool_sprintf(&p, "tx_queue_%u_%.18s",
						i, veth_tq_stats_desc[j].desc);

		page_pool_ethtool_stats_get_strings(p);
		break;
	}
}


int veth_get_sset_count(struct net_device *dev, int sset){
	switch (sset) {
	case ETH_SS_STATS:
		return ARRAY_SIZE(ethtool_stats_keys) +
		       VETH_RQ_STATS_LEN * dev->real_num_rx_queues +
		       VETH_TQ_STATS_LEN * dev->real_num_tx_queues +
		       page_pool_ethtool_stats_get_count();
	default:
		return -EOPNOTSUPP;
	}
}



void veth_get_ethtool_stats(struct net_device *dev,
		struct ethtool_stats *stats, u64 *data){

	struct veth_priv *rcv_priv, *priv = netdev_priv(dev);
	struct net_device *peer = rtnl_dereference(priv->peer);
	int i, j, idx, pp_idx;

	data[0] = peer ? peer->ifindex : 0;
	idx = 1;
	for (i = 0; i < dev->real_num_rx_queues; i++) {
		const struct veth_rq_stats *rq_stats = &priv->rq[i].stats;
		const void *stats_base = (void *)&rq_stats->vs;
		unsigned int start;
		size_t offset;

		do {
			start = u64_stats_fetch_begin(&rq_stats->syncp);
			for (j = 0; j < VETH_RQ_STATS_LEN; j++) {
				offset = veth_rq_stats_desc[j].offset;
				data[idx + j] = *(u64 *)(stats_base + offset);
			}
		} while (u64_stats_fetch_retry(&rq_stats->syncp, start));
		idx += VETH_RQ_STATS_LEN;
	}
	pp_idx = idx;

	if (!peer)
		goto page_pool_stats;

	rcv_priv = netdev_priv(peer);
	for (i = 0; i < peer->real_num_rx_queues; i++) {
		const struct veth_rq_stats *rq_stats = &rcv_priv->rq[i].stats;
		const void *base = (void *)&rq_stats->vs;
		unsigned int start, tx_idx = idx;
		size_t offset;

		tx_idx += (i % dev->real_num_tx_queues) * VETH_TQ_STATS_LEN;
		do {
			start = u64_stats_fetch_begin(&rq_stats->syncp);
			for (j = 0; j < VETH_TQ_STATS_LEN; j++) {
				offset = veth_tq_stats_desc[j].offset;
				data[tx_idx + j] += *(u64 *)(base + offset);
			}
		} while (u64_stats_fetch_retry(&rq_stats->syncp, start));
		pp_idx = tx_idx + VETH_TQ_STATS_LEN;
	}

page_pool_stats:
	veth_get_page_pool_stats(dev, &data[pp_idx]);
}


int veth_get_link_ksettings(struct net_device *dev,
				   struct ethtool_link_ksettings *cmd){

	cmd->base.speed		= SPEED_10000;
	cmd->base.duplex	= DUPLEX_FULL;
	cmd->base.port		= PORT_TP;
	cmd->base.autoneg	= AUTONEG_DISABLE;
	return 0;
}


void veth_get_channels(struct net_device *dev,
			      struct ethtool_channels *channels){

	channels->tx_count = dev->real_num_tx_queues;
	channels->rx_count = dev->real_num_rx_queues;
	channels->max_tx = dev->num_tx_queues;
	channels->max_rx = dev->num_rx_queues;
}



int veth_set_channels(struct net_device *dev,
			     struct ethtool_channels *ch){
                    
	struct veth_priv *priv = netdev_priv(dev);
	unsigned int old_rx_count, new_rx_count;
	struct veth_priv *peer_priv;
	struct net_device *peer;
	int err;

	/* sanity check. Upper bounds are already enforced by the caller */
	if (!ch->rx_count || !ch->tx_count)
		return -EINVAL;

	/* avoid braking XDP, if that is enabled */
	peer = rtnl_dereference(priv->peer);
	peer_priv = peer ? netdev_priv(peer) : NULL;
	if (priv->_xdp_prog && peer && ch->rx_count < peer->real_num_tx_queues)
		return -EINVAL;

	if (peer && peer_priv && peer_priv->_xdp_prog && ch->tx_count > peer->real_num_rx_queues)
		return -EINVAL;

	old_rx_count = dev->real_num_rx_queues;
	new_rx_count = ch->rx_count;
	if (netif_running(dev)) {
		/* turn device off */
		netif_carrier_off(dev);
		if (peer)
			netif_carrier_off(peer);

		/* try to allocate new resurces, as needed*/
		err = veth_enable_range_safe(dev, old_rx_count, new_rx_count);
		if (err)
			goto out;
	}

	err = netif_set_real_num_rx_queues(dev, ch->rx_count);
	if (err)
		goto revert;

	err = netif_set_real_num_tx_queues(dev, ch->tx_count);
	if (err) {
		int err2 = netif_set_real_num_rx_queues(dev, old_rx_count);

		/* this error condition could happen only if rx and tx change
		 * in opposite directions (e.g. tx nr raises, rx nr decreases)
		 * and we can't do anything to fully restore the original
		 * status
		 */
		if (err2)
			pr_warn("Can't restore rx queues config %d -> %d %d",
				new_rx_count, old_rx_count, err2);
		else
			goto revert;
	}

out:
	if (netif_running(dev)) {
		/* note that we need to swap the arguments WRT the enable part
		 * to identify the range we have to disable
		 */
		veth_disable_range_safe(dev, new_rx_count, old_rx_count);
		netif_carrier_on(dev);
		if (peer)
			netif_carrier_on(peer);
	}

	/* update XDP supported features */
	veth_set_xdp_features(dev);
	if (peer)
		veth_set_xdp_features(peer);

	return err;

revert:
	new_rx_count = old_rx_count;
	old_rx_count = ch->rx_count;
	goto out;
}