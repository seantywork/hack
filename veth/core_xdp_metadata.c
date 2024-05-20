#include "veth2.h"


int veth_xdp_rx_timestamp(const struct xdp_md *ctx, u64 *timestamp){

	struct veth_xdp_buff *_ctx = (void *)ctx;

	if (!_ctx->skb)
		return -ENODATA;

	*timestamp = skb_hwtstamps(_ctx->skb)->hwtstamp;
	return 0;
}


int veth_xdp_rx_hash(const struct xdp_md *ctx, u32 *hash,
			    enum xdp_rss_hash_type *rss_type){
                    
	struct veth_xdp_buff *_ctx = (void *)ctx;
	struct sk_buff *skb = _ctx->skb;

	if (!skb)
		return -ENODATA;

	*hash = skb_get_hash(skb);
	*rss_type = skb->l4_hash ? XDP_RSS_TYPE_L4_ANY : XDP_RSS_TYPE_NONE;

	return 0;
}
