

#include "veth2.h"

void veth_get_page_pool_stats(struct net_device *dev, u64 *data){

#ifdef CONFIG_PAGE_POOL_STATS
	struct veth_priv *priv = netdev_priv(dev);
	struct page_pool_stats pp_stats = {};
	int i;

	for (i = 0; i < dev->real_num_rx_queues; i++) {
		if (!priv->rq[i].page_pool)
			continue;
		page_pool_get_stats(priv->rq[i].page_pool, &pp_stats);
	}
	page_pool_ethtool_stats_get(data, &pp_stats);
#endif /* CONFIG_PAGE_POOL_STATS */
}





/* general routines */

bool veth_is_xdp_frame(void *ptr){

	return (unsigned long)ptr & VETH_XDP_FLAG;
}

struct xdp_frame *veth_ptr_to_xdp(void *ptr){

	return (void *)((unsigned long)ptr & ~VETH_XDP_FLAG);
}

void *veth_xdp_to_ptr(struct xdp_frame *xdp){

	return (void *)((unsigned long)xdp | VETH_XDP_FLAG);
}

void veth_ptr_free(void *ptr){

	if (veth_is_xdp_frame(ptr))
		xdp_return_frame(veth_ptr_to_xdp(ptr));
	else
		kfree_skb(ptr);
}

void __veth_xdp_flush(struct veth_rq *rq){

	/* Write ptr_ring before reading rx_notify_masked */
	smp_mb();
	if (!READ_ONCE(rq->rx_notify_masked) &&
	    napi_schedule_prep(&rq->xdp_napi)) {
		WRITE_ONCE(rq->rx_notify_masked, true);
		__napi_schedule(&rq->xdp_napi);
	}
}

int veth_xdp_rx(struct veth_rq *rq, struct sk_buff *skb){

	if (unlikely(ptr_ring_produce(&rq->xdp_ring, skb))) {
		dev_kfree_skb_any(skb);
		return NET_RX_DROP;
	}

	return NET_RX_SUCCESS;
}

int veth_forward_skb(struct net_device *dev, struct sk_buff *skb,
			    struct veth_rq *rq, bool xdp){

	return __dev_forward_skb(dev, skb) ?: xdp ?
		veth_xdp_rx(rq, skb) :
		__netif_rx(skb);
}

/* return true if the specified skb has chances of GRO aggregation
 * Don't strive for accuracy, but try to avoid GRO overhead in the most
 * common scenarios.
 * When XDP is enabled, all traffic is considered eligible, as the xmit
 * device has TSO off.
 * When TSO is enabled on the xmit device, we are likely interested only
 * in UDP aggregation, explicitly check for that if the skb is suspected
 * - the sock_wfree destructor is used by UDP, ICMP and XDP sockets -
 * to belong to locally generated UDP traffic.
 */
bool veth_skb_is_eligible_for_gro(const struct net_device *dev,
					 const struct net_device *rcv,
					 const struct sk_buff *skb){

	return !(dev->features & NETIF_F_ALL_TSO) ||
		(skb->destructor == sock_wfree &&
		 rcv->features & (NETIF_F_GRO_FRAGLIST | NETIF_F_GRO_UDP_FWD));
}


u64 veth_stats_tx(struct net_device *dev, u64 *packets, u64 *bytes){

	struct veth_priv *priv = netdev_priv(dev);

	dev_lstats_read(dev, packets, bytes);
	return atomic64_read(&priv->dropped);
}

void veth_stats_rx(struct veth_stats *result, struct net_device *dev){

	struct veth_priv *priv = netdev_priv(dev);
	int i;

	result->peer_tq_xdp_xmit_err = 0;
	result->xdp_packets = 0;
	result->xdp_tx_err = 0;
	result->xdp_bytes = 0;
	result->rx_drops = 0;
	for (i = 0; i < dev->num_rx_queues; i++) {
		u64 packets, bytes, drops, xdp_tx_err, peer_tq_xdp_xmit_err;
		struct veth_rq_stats *stats = &priv->rq[i].stats;
		unsigned int start;

		do {
			start = u64_stats_fetch_begin(&stats->syncp);
			peer_tq_xdp_xmit_err = stats->vs.peer_tq_xdp_xmit_err;
			xdp_tx_err = stats->vs.xdp_tx_err;
			packets = stats->vs.xdp_packets;
			bytes = stats->vs.xdp_bytes;
			drops = stats->vs.rx_drops;
		} while (u64_stats_fetch_retry(&stats->syncp, start));
		result->peer_tq_xdp_xmit_err += peer_tq_xdp_xmit_err;
		result->xdp_tx_err += xdp_tx_err;
		result->xdp_packets += packets;
		result->xdp_bytes += bytes;
		result->rx_drops += drops;
	}
}



int veth_select_rxq(struct net_device *dev){

	return smp_processor_id() % dev->real_num_rx_queues;
}

int veth_xdp_xmit(struct net_device *dev, int n,
			 struct xdp_frame **frames,
			 u32 flags, bool ndo_xmit){

	struct veth_priv *rcv_priv, *priv = netdev_priv(dev);
	int i, ret = -ENXIO, nxmit = 0;
	struct net_device *rcv;
	unsigned int max_len;
	struct veth_rq *rq;

	if (unlikely(flags & ~XDP_XMIT_FLAGS_MASK))
		return -EINVAL;

	rcu_read_lock();
	rcv = rcu_dereference(priv->peer);
	if (unlikely(!rcv))
		goto out;

	rcv_priv = netdev_priv(rcv);
	rq = &rcv_priv->rq[veth_select_rxq(rcv)];
	/* The napi pointer is set if NAPI is enabled, which ensures that
	 * xdp_ring is initialized on receive side and the peer device is up.
	 */
	if (!rcu_access_pointer(rq->napi))
		goto out;

	max_len = rcv->mtu + rcv->hard_header_len + VLAN_HLEN;

	spin_lock(&rq->xdp_ring.producer_lock);
	for (i = 0; i < n; i++) {
		struct xdp_frame *frame = frames[i];
		void *ptr = veth_xdp_to_ptr(frame);

		if (unlikely(xdp_get_frame_len(frame) > max_len ||
			     __ptr_ring_produce(&rq->xdp_ring, ptr)))
			break;
		nxmit++;
	}
	spin_unlock(&rq->xdp_ring.producer_lock);

	if (flags & XDP_XMIT_FLUSH)
		__veth_xdp_flush(rq);

	ret = nxmit;
	if (ndo_xmit) {
		u64_stats_update_begin(&rq->stats.syncp);
		rq->stats.vs.peer_tq_xdp_xmit += nxmit;
		rq->stats.vs.peer_tq_xdp_xmit_err += n - nxmit;
		u64_stats_update_end(&rq->stats.syncp);
	}

out:
	rcu_read_unlock();

	return ret;
}


void veth_xdp_flush_bq(struct veth_rq *rq, struct veth_xdp_tx_bq *bq){

	int sent, i, err = 0, drops;

	sent = veth_xdp_xmit(rq->dev, bq->count, bq->q, 0, false);
	if (sent < 0) {
		err = sent;
		sent = 0;
	}

	for (i = sent; unlikely(i < bq->count); i++)
		xdp_return_frame(bq->q[i]);

	drops = bq->count - sent;
	trace_xdp_bulk_tx(rq->dev, sent, drops, err);

	u64_stats_update_begin(&rq->stats.syncp);
	rq->stats.vs.xdp_tx += sent;
	rq->stats.vs.xdp_tx_err += drops;
	u64_stats_update_end(&rq->stats.syncp);

	bq->count = 0;
}

void veth_xdp_flush(struct veth_rq *rq, struct veth_xdp_tx_bq *bq){

	struct veth_priv *rcv_priv, *priv = netdev_priv(rq->dev);
	struct net_device *rcv;
	struct veth_rq *rcv_rq;

	rcu_read_lock();
	veth_xdp_flush_bq(rq, bq);
	rcv = rcu_dereference(priv->peer);
	if (unlikely(!rcv))
		goto out;

	rcv_priv = netdev_priv(rcv);
	rcv_rq = &rcv_priv->rq[veth_select_rxq(rcv)];
	/* xdp_ring is initialized on receive side? */
	if (unlikely(!rcu_access_pointer(rcv_rq->xdp_prog)))
		goto out;

	__veth_xdp_flush(rcv_rq);
out:
	rcu_read_unlock();
}

int veth_xdp_tx(struct veth_rq *rq, struct xdp_buff *xdp,
		       struct veth_xdp_tx_bq *bq){
	struct xdp_frame *frame = xdp_convert_buff_to_frame(xdp);

	if (unlikely(!frame))
		return -EOVERFLOW;

	if (unlikely(bq->count == VETH_XDP_TX_BULK_SIZE))
		veth_xdp_flush_bq(rq, bq);

	bq->q[bq->count++] = frame;

	return 0;
}

struct xdp_frame *veth_xdp_rcv_one(struct veth_rq *rq,
					  struct xdp_frame *frame,
					  struct veth_xdp_tx_bq *bq,
					  struct veth_stats *stats){
	struct xdp_frame orig_frame;
	struct bpf_prog *xdp_prog;

	rcu_read_lock();
	xdp_prog = rcu_dereference(rq->xdp_prog);
	if (likely(xdp_prog)) {
		struct veth_xdp_buff vxbuf;
		struct xdp_buff *xdp = &vxbuf.xdp;
		u32 act;

		xdp_convert_frame_to_buff(frame, xdp);
		xdp->rxq = &rq->xdp_rxq;
		vxbuf.skb = NULL;

		act = bpf_prog_run_xdp(xdp_prog, xdp);

		switch (act) {
		case XDP_PASS:
			if (xdp_update_frame_from_buff(xdp, frame))
				goto err_xdp;
			break;
		case XDP_TX:
			orig_frame = *frame;
			xdp->rxq->mem = frame->mem;
			if (unlikely(veth_xdp_tx(rq, xdp, bq) < 0)) {
				trace_xdp_exception(rq->dev, xdp_prog, act);
				frame = &orig_frame;
				stats->rx_drops++;
				goto err_xdp;
			}
			stats->xdp_tx++;
			rcu_read_unlock();
			goto xdp_xmit;
		case XDP_REDIRECT:
			orig_frame = *frame;
			xdp->rxq->mem = frame->mem;
			if (xdp_do_redirect(rq->dev, xdp, xdp_prog)) {
				frame = &orig_frame;
				stats->rx_drops++;
				goto err_xdp;
			}
			stats->xdp_redirect++;
			rcu_read_unlock();
			goto xdp_xmit;
		default:
			bpf_warn_invalid_xdp_action(rq->dev, xdp_prog, act);
			fallthrough;
		case XDP_ABORTED:
			trace_xdp_exception(rq->dev, xdp_prog, act);
			fallthrough;
		case XDP_DROP:
			stats->xdp_drops++;
			goto err_xdp;
		}
	}
	rcu_read_unlock();

	return frame;
err_xdp:
	rcu_read_unlock();
	xdp_return_frame(frame);
xdp_xmit:
	return NULL;
}

/* frames array contains VETH_XDP_BATCH at most */
void veth_xdp_rcv_bulk_skb(struct veth_rq *rq, void **frames,
				  int n_xdpf, struct veth_xdp_tx_bq *bq,
				  struct veth_stats *stats){

	void *skbs[VETH_XDP_BATCH];
	int i;

	if (xdp_alloc_skb_bulk(skbs, n_xdpf,
			       GFP_ATOMIC | __GFP_ZERO) < 0) {
		for (i = 0; i < n_xdpf; i++)
			xdp_return_frame(frames[i]);
		stats->rx_drops += n_xdpf;

		return;
	}

	for (i = 0; i < n_xdpf; i++) {
		struct sk_buff *skb = skbs[i];

		skb = __xdp_build_skb_from_frame(frames[i], skb,
						 rq->dev);
		if (!skb) {
			xdp_return_frame(frames[i]);
			stats->rx_drops++;
			continue;
		}
		napi_gro_receive(&rq->xdp_napi, skb);
	}
}

void veth_xdp_get(struct xdp_buff *xdp){

	struct skb_shared_info *sinfo = xdp_get_shared_info_from_buff(xdp);
	int i;

	get_page(virt_to_page(xdp->data));
	if (likely(!xdp_buff_has_frags(xdp)))
		return;

	for (i = 0; i < sinfo->nr_frags; i++)
		__skb_frag_ref(&sinfo->frags[i]);
}

int veth_convert_skb_to_xdp_buff(struct veth_rq *rq,
					struct xdp_buff *xdp,
					struct sk_buff **pskb){

	struct sk_buff *skb = *pskb;
	u32 frame_sz;

	if (skb_shared(skb) || skb_head_is_locked(skb) ||
	    skb_shinfo(skb)->nr_frags ||
	    skb_headroom(skb) < XDP_PACKET_HEADROOM) {
		u32 size, len, max_head_size, off;
		struct sk_buff *nskb;
		struct page *page;
		int i, head_off;

		/* We need a private copy of the skb and data buffers since
		 * the ebpf program can modify it. We segment the original skb
		 * into order-0 pages without linearize it.
		 *
		 * Make sure we have enough space for linear and paged area
		 */
		max_head_size = SKB_WITH_OVERHEAD(PAGE_SIZE -
						  VETH_XDP_HEADROOM);
		if (skb->len > PAGE_SIZE * MAX_SKB_FRAGS + max_head_size)
			goto drop;

		/* Allocate skb head */
		page = page_pool_dev_alloc_pages(rq->page_pool);
		if (!page)
			goto drop;

		nskb = napi_build_skb(page_address(page), PAGE_SIZE);
		if (!nskb) {
			page_pool_put_full_page(rq->page_pool, page, true);
			goto drop;
		}

		skb_reserve(nskb, VETH_XDP_HEADROOM);
		skb_copy_header(nskb, skb);
		skb_mark_for_recycle(nskb);

		size = min_t(u32, skb->len, max_head_size);
		if (skb_copy_bits(skb, 0, nskb->data, size)) {
			consume_skb(nskb);
			goto drop;
		}
		skb_put(nskb, size);

		head_off = skb_headroom(nskb) - skb_headroom(skb);
		skb_headers_offset_update(nskb, head_off);

		/* Allocate paged area of new skb */
		off = size;
		len = skb->len - off;

		for (i = 0; i < MAX_SKB_FRAGS && off < skb->len; i++) {
			page = page_pool_dev_alloc_pages(rq->page_pool);
			if (!page) {
				consume_skb(nskb);
				goto drop;
			}

			size = min_t(u32, len, PAGE_SIZE);
			skb_add_rx_frag(nskb, i, page, 0, size, PAGE_SIZE);
			if (skb_copy_bits(skb, off, page_address(page),
					  size)) {
				consume_skb(nskb);
				goto drop;
			}

			len -= size;
			off += size;
		}

		consume_skb(skb);
		skb = nskb;
	}

	/* SKB "head" area always have tailroom for skb_shared_info */
	frame_sz = skb_end_pointer(skb) - skb->head;
	frame_sz += SKB_DATA_ALIGN(sizeof(struct skb_shared_info));
	xdp_init_buff(xdp, frame_sz, &rq->xdp_rxq);
	xdp_prepare_buff(xdp, skb->head, skb_headroom(skb),
			 skb_headlen(skb), true);

	if (skb_is_nonlinear(skb)) {
		skb_shinfo(skb)->xdp_frags_size = skb->data_len;
		xdp_buff_set_frags_flag(xdp);
	} else {
		xdp_buff_clear_frags_flag(xdp);
	}
	*pskb = skb;

	return 0;
drop:
	consume_skb(skb);
	*pskb = NULL;

	return -ENOMEM;
}

struct sk_buff *veth_xdp_rcv_skb(struct veth_rq *rq,
					struct sk_buff *skb,
					struct veth_xdp_tx_bq *bq,
					struct veth_stats *stats){

	void *orig_data, *orig_data_end;
	struct bpf_prog *xdp_prog;
	struct veth_xdp_buff vxbuf;
	struct xdp_buff *xdp = &vxbuf.xdp;
	u32 act, metalen;
	int off;

	skb_prepare_for_gro(skb);

	rcu_read_lock();
	xdp_prog = rcu_dereference(rq->xdp_prog);
	if (unlikely(!xdp_prog)) {
		rcu_read_unlock();
		goto out;
	}

	__skb_push(skb, skb->data - skb_mac_header(skb));
	if (veth_convert_skb_to_xdp_buff(rq, xdp, &skb))
		goto drop;
	vxbuf.skb = skb;

	orig_data = xdp->data;
	orig_data_end = xdp->data_end;

	act = bpf_prog_run_xdp(xdp_prog, xdp);

	switch (act) {
	case XDP_PASS:
		break;
	case XDP_TX:
		veth_xdp_get(xdp);
		consume_skb(skb);
		xdp->rxq->mem = rq->xdp_mem;
		if (unlikely(veth_xdp_tx(rq, xdp, bq) < 0)) {
			trace_xdp_exception(rq->dev, xdp_prog, act);
			stats->rx_drops++;
			goto err_xdp;
		}
		stats->xdp_tx++;
		rcu_read_unlock();
		goto xdp_xmit;
	case XDP_REDIRECT:
		veth_xdp_get(xdp);
		consume_skb(skb);
		xdp->rxq->mem = rq->xdp_mem;
		if (xdp_do_redirect(rq->dev, xdp, xdp_prog)) {
			stats->rx_drops++;
			goto err_xdp;
		}
		stats->xdp_redirect++;
		rcu_read_unlock();
		goto xdp_xmit;
	default:
		bpf_warn_invalid_xdp_action(rq->dev, xdp_prog, act);
		fallthrough;
	case XDP_ABORTED:
		trace_xdp_exception(rq->dev, xdp_prog, act);
		fallthrough;
	case XDP_DROP:
		stats->xdp_drops++;
		goto xdp_drop;
	}
	rcu_read_unlock();

	/* check if bpf_xdp_adjust_head was used */
	off = orig_data - xdp->data;
	if (off > 0)
		__skb_push(skb, off);
	else if (off < 0)
		__skb_pull(skb, -off);

	skb_reset_mac_header(skb);

	/* check if bpf_xdp_adjust_tail was used */
	off = xdp->data_end - orig_data_end;
	if (off != 0)
		__skb_put(skb, off); /* positive on grow, negative on shrink */

	/* XDP frag metadata (e.g. nr_frags) are updated in eBPF helpers
	 * (e.g. bpf_xdp_adjust_tail), we need to update data_len here.
	 */
	if (xdp_buff_has_frags(xdp))
		skb->data_len = skb_shinfo(skb)->xdp_frags_size;
	else
		skb->data_len = 0;

	skb->protocol = eth_type_trans(skb, rq->dev);

	metalen = xdp->data - xdp->data_meta;
	if (metalen)
		skb_metadata_set(skb, metalen);
out:
	return skb;
drop:
	stats->rx_drops++;
xdp_drop:
	rcu_read_unlock();
	kfree_skb(skb);
	return NULL;
err_xdp:
	rcu_read_unlock();
	xdp_return_buff(xdp);
xdp_xmit:
	return NULL;
}

int veth_xdp_rcv(struct veth_rq *rq, int budget,
			struct veth_xdp_tx_bq *bq,
			struct veth_stats *stats){

	int i, done = 0, n_xdpf = 0;
	void *xdpf[VETH_XDP_BATCH];

	for (i = 0; i < budget; i++) {
		void *ptr = __ptr_ring_consume(&rq->xdp_ring);

		if (!ptr)
			break;

		if (veth_is_xdp_frame(ptr)) {
			/* ndo_xdp_xmit */
			struct xdp_frame *frame = veth_ptr_to_xdp(ptr);

			stats->xdp_bytes += xdp_get_frame_len(frame);
			frame = veth_xdp_rcv_one(rq, frame, bq, stats);
			if (frame) {
				/* XDP_PASS */
				xdpf[n_xdpf++] = frame;
				if (n_xdpf == VETH_XDP_BATCH) {
					veth_xdp_rcv_bulk_skb(rq, xdpf, n_xdpf,
							      bq, stats);
					n_xdpf = 0;
				}
			}
		} else {
			/* ndo_start_xmit */
			struct sk_buff *skb = ptr;

			stats->xdp_bytes += skb->len;
			skb = veth_xdp_rcv_skb(rq, skb, bq, stats);
			if (skb) {
				if (skb_shared(skb) || skb_unclone(skb, GFP_ATOMIC))
					netif_receive_skb(skb);
				else
					napi_gro_receive(&rq->xdp_napi, skb);
			}
		}
		done++;
	}

	if (n_xdpf)
		veth_xdp_rcv_bulk_skb(rq, xdpf, n_xdpf, bq, stats);

	u64_stats_update_begin(&rq->stats.syncp);
	rq->stats.vs.xdp_redirect += stats->xdp_redirect;
	rq->stats.vs.xdp_bytes += stats->xdp_bytes;
	rq->stats.vs.xdp_drops += stats->xdp_drops;
	rq->stats.vs.rx_drops += stats->rx_drops;
	rq->stats.vs.xdp_packets += done;
	u64_stats_update_end(&rq->stats.syncp);

	return done;
}

int veth_poll(struct napi_struct *napi, int budget){

	struct veth_rq *rq =
		container_of(napi, struct veth_rq, xdp_napi);
	struct veth_stats stats = {};
	struct veth_xdp_tx_bq bq;
	int done;

	bq.count = 0;

	xdp_set_return_frame_no_direct();
	done = veth_xdp_rcv(rq, budget, &bq, &stats);

	if (stats.xdp_redirect > 0)
		xdp_do_flush();

	if (done < budget && napi_complete_done(napi, done)) {
		/* Write rx_notify_masked before reading ptr_ring */
		smp_store_mb(rq->rx_notify_masked, false);
		if (unlikely(!__ptr_ring_empty(&rq->xdp_ring))) {
			if (napi_schedule_prep(&rq->xdp_napi)) {
				WRITE_ONCE(rq->rx_notify_masked, true);
				__napi_schedule(&rq->xdp_napi);
			}
		}
	}

	if (stats.xdp_tx > 0)
		veth_xdp_flush(rq, &bq);
	xdp_clear_return_frame_no_direct();

	return done;
}

int veth_create_page_pool(struct veth_rq *rq){

	struct page_pool_params pp_params = {
		.order = 0,
		.pool_size = VETH_RING_SIZE,
		.nid = NUMA_NO_NODE,
		.dev = &rq->dev->dev,
	};

	rq->page_pool = page_pool_create(&pp_params);
	if (IS_ERR(rq->page_pool)) {
		int err = PTR_ERR(rq->page_pool);

		rq->page_pool = NULL;
		return err;
	}

	return 0;
}

int __veth_napi_enable_range(struct net_device *dev, int start, int end){

	struct veth_priv *priv = netdev_priv(dev);
	int err, i;

	for (i = start; i < end; i++) {
		err = veth_create_page_pool(&priv->rq[i]);
		if (err)
			goto err_page_pool;
	}

	for (i = start; i < end; i++) {
		struct veth_rq *rq = &priv->rq[i];

		err = ptr_ring_init(&rq->xdp_ring, VETH_RING_SIZE, GFP_KERNEL);
		if (err)
			goto err_xdp_ring;
	}

	for (i = start; i < end; i++) {
		struct veth_rq *rq = &priv->rq[i];

		napi_enable(&rq->xdp_napi);
		rcu_assign_pointer(priv->rq[i].napi, &priv->rq[i].xdp_napi);
	}

	return 0;

err_xdp_ring:
	for (i--; i >= start; i--)
		ptr_ring_cleanup(&priv->rq[i].xdp_ring, veth_ptr_free);
	i = end;
err_page_pool:
	for (i--; i >= start; i--) {
		page_pool_destroy(priv->rq[i].page_pool);
		priv->rq[i].page_pool = NULL;
	}

	return err;
}

int __veth_napi_enable(struct net_device *dev){

	return __veth_napi_enable_range(dev, 0, dev->real_num_rx_queues);
}

void veth_napi_del_range(struct net_device *dev, int start, int end){

	struct veth_priv *priv = netdev_priv(dev);
	int i;

	for (i = start; i < end; i++) {
		struct veth_rq *rq = &priv->rq[i];

		rcu_assign_pointer(priv->rq[i].napi, NULL);
		napi_disable(&rq->xdp_napi);
		__netif_napi_del(&rq->xdp_napi);
	}
	synchronize_net();

	for (i = start; i < end; i++) {
		struct veth_rq *rq = &priv->rq[i];

		rq->rx_notify_masked = false;
		ptr_ring_cleanup(&rq->xdp_ring, veth_ptr_free);
	}

	for (i = start; i < end; i++) {
		page_pool_destroy(priv->rq[i].page_pool);
		priv->rq[i].page_pool = NULL;
	}
}

void veth_napi_del(struct net_device *dev){

	veth_napi_del_range(dev, 0, dev->real_num_rx_queues);
}

bool veth_gro_requested(const struct net_device *dev){

	return !!(dev->wanted_features & NETIF_F_GRO);
}

int veth_enable_xdp_range(struct net_device *dev, int start, int end,
				 bool napi_already_on){

	struct veth_priv *priv = netdev_priv(dev);
	int err, i;

	for (i = start; i < end; i++) {
		struct veth_rq *rq = &priv->rq[i];

		if (!napi_already_on)
			netif_napi_add(dev, &rq->xdp_napi, veth_poll);
		err = xdp_rxq_info_reg(&rq->xdp_rxq, dev, i, rq->xdp_napi.napi_id);
		if (err < 0)
			goto err_rxq_reg;

		err = xdp_rxq_info_reg_mem_model(&rq->xdp_rxq,
						 MEM_TYPE_PAGE_SHARED,
						 NULL);
		if (err < 0)
			goto err_reg_mem;

		/* Save original mem info as it can be overwritten */
		rq->xdp_mem = rq->xdp_rxq.mem;
	}
	return 0;

err_reg_mem:
	xdp_rxq_info_unreg(&priv->rq[i].xdp_rxq);
err_rxq_reg:
	for (i--; i >= start; i--) {
		struct veth_rq *rq = &priv->rq[i];

		xdp_rxq_info_unreg(&rq->xdp_rxq);
		if (!napi_already_on)
			netif_napi_del(&rq->xdp_napi);
	}

	return err;
}

void veth_disable_xdp_range(struct net_device *dev, int start, int end,
				   bool delete_napi){

	struct veth_priv *priv = netdev_priv(dev);
	int i;

	for (i = start; i < end; i++) {
		struct veth_rq *rq = &priv->rq[i];

		rq->xdp_rxq.mem = rq->xdp_mem;
		xdp_rxq_info_unreg(&rq->xdp_rxq);

		if (delete_napi)
			netif_napi_del(&rq->xdp_napi);
	}
}

int veth_enable_xdp(struct net_device *dev){

	bool napi_already_on = veth_gro_requested(dev) && (dev->flags & IFF_UP);
	struct veth_priv *priv = netdev_priv(dev);
	int err, i;

	if (!xdp_rxq_info_is_reg(&priv->rq[0].xdp_rxq)) {
		err = veth_enable_xdp_range(dev, 0, dev->real_num_rx_queues, napi_already_on);
		if (err)
			return err;

		if (!napi_already_on) {
			err = __veth_napi_enable(dev);
			if (err) {
				veth_disable_xdp_range(dev, 0, dev->real_num_rx_queues, true);
				return err;
			}

			if (!veth_gro_requested(dev)) {
				/* user-space did not require GRO, but adding XDP
				 * is supposed to get GRO working
				 */
				dev->features |= NETIF_F_GRO;
				netdev_features_change(dev);
			}
		}
	}

	for (i = 0; i < dev->real_num_rx_queues; i++) {
		rcu_assign_pointer(priv->rq[i].xdp_prog, priv->_xdp_prog);
		rcu_assign_pointer(priv->rq[i].napi, &priv->rq[i].xdp_napi);
	}

	return 0;
}

void veth_disable_xdp(struct net_device *dev){

	struct veth_priv *priv = netdev_priv(dev);
	int i;

	for (i = 0; i < dev->real_num_rx_queues; i++)
		rcu_assign_pointer(priv->rq[i].xdp_prog, NULL);

	if (!netif_running(dev) || !veth_gro_requested(dev)) {
		veth_napi_del(dev);

		/* if user-space did not require GRO, since adding XDP
		 * enabled it, clear it now
		 */
		if (!veth_gro_requested(dev) && netif_running(dev)) {
			dev->features &= ~NETIF_F_GRO;
			netdev_features_change(dev);
		}
	}

	veth_disable_xdp_range(dev, 0, dev->real_num_rx_queues, false);
}

int veth_napi_enable_range(struct net_device *dev, int start, int end){

	struct veth_priv *priv = netdev_priv(dev);
	int err, i;

	for (i = start; i < end; i++) {
		struct veth_rq *rq = &priv->rq[i];

		netif_napi_add(dev, &rq->xdp_napi, veth_poll);
	}

	err = __veth_napi_enable_range(dev, start, end);
	if (err) {
		for (i = start; i < end; i++) {
			struct veth_rq *rq = &priv->rq[i];

			netif_napi_del(&rq->xdp_napi);
		}
		return err;
	}
	return err;
}

int veth_napi_enable(struct net_device *dev){

	return veth_napi_enable_range(dev, 0, dev->real_num_rx_queues);
}

void veth_disable_range_safe(struct net_device *dev, int start, int end){

	struct veth_priv *priv = netdev_priv(dev);

	if (start >= end)
		return;

	if (priv->_xdp_prog) {
		veth_napi_del_range(dev, start, end);
		veth_disable_xdp_range(dev, start, end, false);
	} else if (veth_gro_requested(dev)) {
		veth_napi_del_range(dev, start, end);
	}
}

int veth_enable_range_safe(struct net_device *dev, int start, int end){

	struct veth_priv *priv = netdev_priv(dev);
	int err;

	if (start >= end)
		return 0;

	if (priv->_xdp_prog) {
		/* these channels are freshly initialized, napi is not on there even
		 * when GRO is requeste
		 */
		err = veth_enable_xdp_range(dev, start, end, false);
		if (err)
			return err;

		err = __veth_napi_enable_range(dev, start, end);
		if (err) {
			/* on error always delete the newly added napis */
			veth_disable_xdp_range(dev, start, end, true);
			return err;
		}
	} else if (veth_gro_requested(dev)) {
		return veth_napi_enable_range(dev, start, end);
	}
	return 0;
}

void veth_set_xdp_features(struct net_device *dev){

	struct veth_priv *priv = netdev_priv(dev);
	struct net_device *peer;

	peer = rtnl_dereference(priv->peer);
	if (peer && peer->real_num_tx_queues <= dev->real_num_rx_queues) {
		struct veth_priv *priv_peer = netdev_priv(peer);
		xdp_features_t val = NETDEV_XDP_ACT_BASIC |
				     NETDEV_XDP_ACT_REDIRECT |
				     NETDEV_XDP_ACT_RX_SG;

		if (priv_peer->_xdp_prog || veth_gro_requested(peer))
			val |= NETDEV_XDP_ACT_NDO_XMIT |
			       NETDEV_XDP_ACT_NDO_XMIT_SG;
		xdp_set_features_flag(dev, val);
	} else {
		xdp_clear_features_flag(dev);
	}
}



int is_valid_veth_mtu(int mtu){

	return mtu >= ETH_MIN_MTU && mtu <= ETH_MAX_MTU;
}

int veth_alloc_queues(struct net_device *dev){

	struct veth_priv *priv = netdev_priv(dev);
	int i;

	priv->rq = kcalloc(dev->num_rx_queues, sizeof(*priv->rq), GFP_KERNEL_ACCOUNT);
	if (!priv->rq)
		return -ENOMEM;

	for (i = 0; i < dev->num_rx_queues; i++) {
		priv->rq[i].dev = dev;
		u64_stats_init(&priv->rq[i].stats.syncp);
	}

	return 0;
}

void veth_free_queues(struct net_device *dev){

	struct veth_priv *priv = netdev_priv(dev);

	kfree(priv->rq);
}



void veth_dev_free(struct net_device *dev){

	veth_free_queues(dev);
	free_percpu(dev->lstats);
}




int veth_xdp_set(struct net_device *dev, struct bpf_prog *prog,
			struct netlink_ext_ack *extack){

	struct veth_priv *priv = netdev_priv(dev);
	struct bpf_prog *old_prog;
	struct net_device *peer;
	unsigned int max_mtu;
	int err;

	old_prog = priv->_xdp_prog;
	priv->_xdp_prog = prog;
	peer = rtnl_dereference(priv->peer);

	if (prog) {
		if (!peer) {
			NL_SET_ERR_MSG_MOD(extack, "Cannot set XDP when peer is detached");
			err = -ENOTCONN;
			goto err;
		}

		max_mtu = SKB_WITH_OVERHEAD(PAGE_SIZE - VETH_XDP_HEADROOM) -
			  peer->hard_header_len;
		/* Allow increasing the max_mtu if the program supports
		 * XDP fragments.
		 */
		if (prog->aux->xdp_has_frags)
			max_mtu += PAGE_SIZE * MAX_SKB_FRAGS;

		if (peer->mtu > max_mtu) {
			NL_SET_ERR_MSG_MOD(extack, "Peer MTU is too large to set XDP");
			err = -ERANGE;
			goto err;
		}

		if (dev->real_num_rx_queues < peer->real_num_tx_queues) {
			NL_SET_ERR_MSG_MOD(extack, "XDP expects number of rx queues not less than peer tx queues");
			err = -ENOSPC;
			goto err;
		}

		if (dev->flags & IFF_UP) {
			err = veth_enable_xdp(dev);
			if (err) {
				NL_SET_ERR_MSG_MOD(extack, "Setup for XDP failed");
				goto err;
			}
		}

		if (!old_prog) {
			peer->hw_features &= ~NETIF_F_GSO_SOFTWARE;
			peer->max_mtu = max_mtu;
		}

		xdp_features_set_redirect_target(peer, true);
	}

	if (old_prog) {
		if (!prog) {
			if (peer && !veth_gro_requested(dev))
				xdp_features_clear_redirect_target(peer);

			if (dev->flags & IFF_UP)
				veth_disable_xdp(dev);

			if (peer) {
				peer->hw_features |= NETIF_F_GSO_SOFTWARE;
				peer->max_mtu = ETH_MAX_MTU;
			}
		}
		bpf_prog_put(old_prog);
	}

	if ((!!old_prog ^ !!prog) && peer)
		netdev_update_features(peer);

	return 0;
err:
	priv->_xdp_prog = old_prog;

	return err;
}

void veth_disable_gro(struct net_device *dev){

	dev->features &= ~NETIF_F_GRO;
	dev->wanted_features &= ~NETIF_F_GRO;
	netdev_update_features(dev);
}

int veth_init_queues(struct net_device *dev, struct nlattr *tb[]){
	int err;

	if (!tb[IFLA_NUM_TX_QUEUES] && dev->num_tx_queues > 1) {
		err = netif_set_real_num_tx_queues(dev, 1);
		if (err)
			return err;
	}
	if (!tb[IFLA_NUM_RX_QUEUES] && dev->num_rx_queues > 1) {
		err = netif_set_real_num_rx_queues(dev, 1);
		if (err)
			return err;
	}
	return 0;
}
