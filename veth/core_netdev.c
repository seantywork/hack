#include "veth2.h"


int veth_dev_init(struct net_device *dev){

	int err;

	dev->lstats = netdev_alloc_pcpu_stats(struct pcpu_lstats);
	if (!dev->lstats)
		return -ENOMEM;

	err = veth_alloc_queues(dev);
	if (err) {
		free_percpu(dev->lstats);
		return err;
	}

	return 0;
}



int veth_open(struct net_device *dev){

	struct veth_priv *priv = netdev_priv(dev);
	struct net_device *peer = rtnl_dereference(priv->peer);
	int err;

	if (!peer)
		return -ENOTCONN;

	if (priv->_xdp_prog) {
		err = veth_enable_xdp(dev);
		if (err)
			return err;
	} else if (veth_gro_requested(dev)) {
		err = veth_napi_enable(dev);
		if (err)
			return err;
	}

	if (peer->flags & IFF_UP) {
		netif_carrier_on(dev);
		netif_carrier_on(peer);
	}

	return 0;
}


int veth_close(struct net_device *dev){

	struct veth_priv *priv = netdev_priv(dev);
	struct net_device *peer = rtnl_dereference(priv->peer);

	netif_carrier_off(dev);
	if (peer)
		netif_carrier_off(peer);

	if (priv->_xdp_prog)
		veth_disable_xdp(dev);
	else if (veth_gro_requested(dev))
		veth_napi_del(dev);

	return 0;
}



netdev_tx_t veth_xmit(struct sk_buff *skb, struct net_device *dev){

	struct veth_priv *rcv_priv, *priv = netdev_priv(dev);
	struct veth_rq *rq = NULL;
	struct net_device *rcv;
	int length = skb->len;
	bool use_napi = false;
	int rxq;

	rcu_read_lock();
	rcv = rcu_dereference(priv->peer);
	if (unlikely(!rcv) || !pskb_may_pull(skb, ETH_HLEN)) {
		kfree_skb(skb);
		goto drop;
	}

	rcv_priv = netdev_priv(rcv);
	rxq = skb_get_queue_mapping(skb);
	if (rxq < rcv->real_num_rx_queues) {
		rq = &rcv_priv->rq[rxq];

		/* The napi pointer is available when an XDP program is
		 * attached or when GRO is enabled
		 * Don't bother with napi/GRO if the skb can't be aggregated
		 */
		use_napi = rcu_access_pointer(rq->napi) &&
			   veth_skb_is_eligible_for_gro(dev, rcv, skb);
	}

	skb_tx_timestamp(skb);
	if (likely(veth_forward_skb(rcv, skb, rq, use_napi) == NET_RX_SUCCESS)) {
		if (!use_napi)
			dev_lstats_add(dev, length);
	} else {
drop:
		atomic64_inc(&priv->dropped);
	}

	if (use_napi)
		__veth_xdp_flush(rq);

	rcu_read_unlock();

	return NETDEV_TX_OK;
}



void veth_get_stats64(struct net_device *dev,
			     struct rtnl_link_stats64 *tot){

	struct veth_priv *priv = netdev_priv(dev);
	struct net_device *peer;
	struct veth_stats rx;
	u64 packets, bytes;

	tot->tx_dropped = veth_stats_tx(dev, &packets, &bytes);
	tot->tx_bytes = bytes;
	tot->tx_packets = packets;

	veth_stats_rx(&rx, dev);
	tot->tx_dropped += rx.xdp_tx_err;
	tot->rx_dropped = rx.rx_drops + rx.peer_tq_xdp_xmit_err;
	tot->rx_bytes = rx.xdp_bytes;
	tot->rx_packets = rx.xdp_packets;

	rcu_read_lock();
	peer = rcu_dereference(priv->peer);
	if (peer) {
		veth_stats_tx(peer, &packets, &bytes);
		tot->rx_bytes += bytes;
		tot->rx_packets += packets;

		veth_stats_rx(&rx, peer);
		tot->tx_dropped += rx.peer_tq_xdp_xmit_err;
		tot->rx_dropped += rx.xdp_tx_err;
		tot->tx_bytes += rx.xdp_bytes;
		tot->tx_packets += rx.xdp_packets;
	}
	rcu_read_unlock();
}


void veth_set_multicast_list(struct net_device *dev){

}

#ifdef CONFIG_NET_POLL_CONTROLLER
void veth_poll_controller(struct net_device *dev)
{
	/* veth only receives frames when its peer sends one
	 * Since it has nothing to do with disabling irqs, we are guaranteed
	 * never to have pending data when we poll for it so
	 * there is nothing to do here.
	 *
	 * We need this though so netpoll recognizes us as an interface that
	 * supports polling, which enables bridge devices in virt setups to
	 * still use netconsole
	 */
}
#endif	/* CONFIG_NET_POLL_CONTROLLER */


int veth_get_iflink(const struct net_device *dev){


	struct veth_priv *priv = netdev_priv(dev);
	struct net_device *peer;
	int iflink;

	rcu_read_lock();
	peer = rcu_dereference(priv->peer);
	iflink = peer ? peer->ifindex : 0;
	rcu_read_unlock();

	return iflink;
}



netdev_features_t veth_fix_features(struct net_device *dev,
					   netdev_features_t features){

	struct veth_priv *priv = netdev_priv(dev);
	struct net_device *peer;

	peer = rtnl_dereference(priv->peer);
	if (peer) {
		struct veth_priv *peer_priv = netdev_priv(peer);

		if (peer_priv->_xdp_prog)
			features &= ~NETIF_F_GSO_SOFTWARE;
	}
	if (priv->_xdp_prog)
		features |= NETIF_F_GRO;

	return features;
}



int veth_set_features(struct net_device *dev,
			     netdev_features_t features){

	netdev_features_t changed = features ^ dev->features;
	struct veth_priv *priv = netdev_priv(dev);
	struct net_device *peer;
	int err;

	if (!(changed & NETIF_F_GRO) || !(dev->flags & IFF_UP) || priv->_xdp_prog)
		return 0;

	peer = rtnl_dereference(priv->peer);
	if (features & NETIF_F_GRO) {
		err = veth_napi_enable(dev);
		if (err)
			return err;

		if (peer)
			xdp_features_set_redirect_target(peer, true);
	} else {
		if (peer)
			xdp_features_clear_redirect_target(peer);
		veth_napi_del(dev);
	}
	return 0;
}



void veth_set_rx_headroom(struct net_device *dev, int new_hr){

	struct veth_priv *peer_priv, *priv = netdev_priv(dev);
	struct net_device *peer;

	if (new_hr < 0)
		new_hr = 0;

	rcu_read_lock();
	peer = rcu_dereference(priv->peer);
	if (unlikely(!peer))
		goto out;

	peer_priv = netdev_priv(peer);
	priv->requested_headroom = new_hr;
	new_hr = max(priv->requested_headroom, peer_priv->requested_headroom);
	dev->needed_headroom = new_hr;
	peer->needed_headroom = new_hr;

out:
	rcu_read_unlock();
}

int veth_xdp(struct net_device *dev, struct netdev_bpf *xdp){

	switch (xdp->command) {
	case XDP_SETUP_PROG:
		return veth_xdp_set(dev, xdp->prog, xdp->extack);
	default:
		return -EINVAL;
	}
}


int veth_ndo_xdp_xmit(struct net_device *dev, int n,
			     struct xdp_frame **frames, u32 flags){

	int err;

	err = veth_xdp_xmit(dev, n, frames, flags, true);
	if (err < 0) {
		struct veth_priv *priv = netdev_priv(dev);

		atomic64_add(n, &priv->dropped);
	}

	return err;
}


struct net_device *veth_peer_dev(struct net_device *dev){
    
	struct veth_priv *priv = netdev_priv(dev);

	/* Callers must be under RCU read side. */
	return rcu_dereference(priv->peer);
}