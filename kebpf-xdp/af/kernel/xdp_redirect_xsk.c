
#define AF_INET		2	/* Internet IP Protocol 	*/
#define ETH_ALEN    6

#include <linux/bpf.h>

#include <bpf/bpf_helpers.h>
#include <bpf/bpf_endian.h>

#include <linux/if_ether.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/udp.h>
#include <linux/in.h>
#include <linux/string.h>

struct {
	__uint(type, BPF_MAP_TYPE_XSKMAP);
	__type(key, __u32);
	__type(value, __u32);
	__uint(max_entries, 64);
} if_redirect SEC(".maps");

/*
struct {
	__uint(type, BPF_MAP_TYPE_PERCPU_ARRAY);
	__type(key, __u32);
	__type(value, __u32);
	__uint(max_entries, 64);
} xdp_stats_map SEC(".maps");
*/

SEC("xdp_redirect_xsk")
int xdp_sock_prog(struct xdp_md *ctx)
{
    int index = ctx->rx_queue_index;

    /*
    pkt_count = bpf_map_lookup_elem(&xdp_stats_map, &index);
    if (pkt_count) {

        if ((*pkt_count)++ & 1)
            return XDP_PASS;
    }
    */
    
    struct bpf_fib_lookup fib_params;

    void *data_end = (void *)(long)ctx->data_end;
    void *data = (void *)(long)ctx->data;
    struct ethhdr *eth_header;
    struct iphdr *ip_header;
    struct tcphdr *tcp_header;
    struct udphdr *udp_header;

    int* val = NULL;

    eth_header = data;

    if ((void *)eth_header + sizeof(*eth_header) > data_end) {
        return XDP_PASS;
    }

    __u16 h_proto = eth_header->h_proto;

    /* anything that is not IPv4 (including ARP) goes up to the kernel */
    if (h_proto != 0x08U) {  // htons(ETH_P_IP) -> 0x08U
        return XDP_PASS;
    }

    ip_header = data + sizeof(*eth_header);
    if ((void *)ip_header + sizeof(*ip_header) > data_end) {

        bpf_printk("packet len too short: pass\n");

        return XDP_PASS;
    }


    if(ip_header->protocol == IPPROTO_ICMP){

        bpf_printk("xsk: proto: icmp\n");

        
       __builtin_memset(&fib_params, 0, sizeof(fib_params));

        fib_params.family	= AF_INET;
        // use daddr as source in the lookup, so we refleect packet back (as if it wcame from us)
        fib_params.ipv4_src	= ip_header->daddr;
        // opposite here, the destination is the source of the icmp packet..remote end
        fib_params.ipv4_dst	= ip_header->saddr;
        fib_params.ifindex = ctx->ingress_ifindex;

        bpf_printk("doing route lookup dst: %d\n", fib_params.ipv4_dst);
        int rc = bpf_fib_lookup(ctx, &fib_params, sizeof(fib_params), 0);
        if ((rc != BPF_FIB_LKUP_RET_SUCCESS) && (rc != BPF_FIB_LKUP_RET_NO_NEIGH)) {
            bpf_printk("dropping packet\n");
            return XDP_DROP;
        } else if (rc == BPF_FIB_LKUP_RET_NO_NEIGH) {
            // here we should let packet pass so we resolve arp.
            bpf_printk("passing packet, lookup returned: %d\n", BPF_FIB_LKUP_RET_NO_NEIGH);
            return XDP_PASS;
        }
        bpf_printk("route lookup success, ifindex: %d\n", fib_params.ifindex);

        __u32 oldipdst = ip_header->daddr;
        ip_header->daddr = ip_header->saddr;
        ip_header->saddr = oldipdst;

        memcpy(eth_header->h_dest, fib_params.dmac, ETH_ALEN);
        memcpy(eth_header->h_source, fib_params.smac, ETH_ALEN);

    } else if (ip_header->protocol == IPPROTO_TCP){

        bpf_printk("xsk: proto: tcp\n");

        
        tcp_header = data + sizeof(*eth_header) + sizeof(*ip_header);
        
       __builtin_memset(&fib_params, 0, sizeof(fib_params));

        fib_params.family	= AF_INET;
        // use daddr as source in the lookup, so we refleect packet back (as if it wcame from us)
        
        fib_params.ipv4_src	= ip_header->saddr;
        //fib_params.ipv4_src = bpf_ntohl(3232243969);
        // opposite here, the destination is the source of the icmp packet..remote end
        fib_params.ipv4_dst	= ip_header->daddr;

        //fib_params.sport = tcp_header->source;
        //fib_params.dport = tcp_header->dest;


        fib_params.ifindex = ctx->ingress_ifindex;

        
        bpf_printk("doing route lookup dst: %d\n", fib_params.ipv4_dst);
        int rc = bpf_fib_lookup(ctx, &fib_params, sizeof(fib_params), 0);
        if ((rc != BPF_FIB_LKUP_RET_SUCCESS) && (rc != BPF_FIB_LKUP_RET_NO_NEIGH)) {
            bpf_printk("dropping packet\n");
            return XDP_DROP;
        } else if (rc == BPF_FIB_LKUP_RET_NO_NEIGH) {
            // here we should let packet pass so we resolve arp.
            bpf_printk("passing packet, lookup returned: %d\n", BPF_FIB_LKUP_RET_NO_NEIGH);
            return XDP_PASS;
        }
        bpf_printk("route lookup success, ifindex: %d\n", fib_params.ifindex);

        //ip_header->daddr = fib_params.ipv4_dst;
        //ip_header->saddr = fib_params.ipv4_src;

        memcpy(eth_header->h_dest, fib_params.dmac, ETH_ALEN);
        memcpy(eth_header->h_source, fib_params.smac, ETH_ALEN);        
        

    } else if (ip_header->protocol == IPPROTO_UDP){

        bpf_printk("xsk: proto: udp\n");
        
        
        udp_header = data + sizeof(*eth_header) + sizeof(*ip_header);
                
       __builtin_memset(&fib_params, 0, sizeof(fib_params));

        fib_params.family	= AF_INET;
        // use daddr as source in the lookup, so we refleect packet back (as if it wcame from us)
        fib_params.ipv4_src	= ip_header->saddr;
        //fib_params.ipv4_src = bpf_ntohl(3232243969);
        // opposite here, the destination is the source of the icmp packet..remote end
        fib_params.ipv4_dst	= ip_header->daddr;

        //fib_params.sport = udp_header->source;
        //fib_params.dport = udp_header->dest;

        //fib_params.l4_protocol = IPPROTO_UDP;

        fib_params.ifindex = ctx->ingress_ifindex;

        bpf_printk("doing route lookup dst: %d\n", fib_params.ipv4_dst);
        int rc = bpf_fib_lookup(ctx, &fib_params, sizeof(fib_params), 0);
        if ((rc != BPF_FIB_LKUP_RET_SUCCESS) && (rc != BPF_FIB_LKUP_RET_NO_NEIGH)) {
            bpf_printk("dropping packet\n");
            return XDP_DROP;
        } else if (rc == BPF_FIB_LKUP_RET_NO_NEIGH) {
            // here we should let packet pass so we resolve arp.
            bpf_printk("passing packet, lookup returned: %d\n", BPF_FIB_LKUP_RET_NO_NEIGH);
            return XDP_PASS;
        }
        bpf_printk("route lookup success, ifindex: %d\n", fib_params.ifindex);

        //ip_header->daddr = fib_params.ipv4_dst;
        //ip_header->saddr = fib_params.ipv4_src;

    
        memcpy(eth_header->h_dest, fib_params.dmac, ETH_ALEN);
        memcpy(eth_header->h_source, fib_params.smac, ETH_ALEN);  
 
    }
/*
    bpf_printk("xsk: mac to use as src: %02x:%02x:%02x:%02x:%02x:%02x\n", 
        eth_header->h_source[0], 
        eth_header->h_source[1], 
        eth_header->h_source[2], 
        eth_header->h_source[3], 
        eth_header->h_source[4],
        eth_header->h_source[5]
        );

    bpf_printk("xsk: mac to use as dst: %02x:%02x:%02x:%02x:%02x:%02x\n", 
        eth_header->h_dest[0], 
        eth_header->h_dest[1], 
        eth_header->h_dest[2], 
        eth_header->h_dest[3], 
        eth_header->h_dest[4],
        eth_header->h_dest[5]
        );
*/

    val = bpf_map_lookup_elem(&if_redirect, &index);

    if (val){

        bpf_printk("val read from the map\n");

    } else {

        bpf_printk("failed to read val from the map\n");

        return XDP_ABORTED;

    }

    int action = bpf_redirect_map(&if_redirect, index, 0);

    if (action == XDP_ABORTED){

        bpf_printk("xsk: aborted\n");
        return action;
    }

    if(action == XDP_REDIRECT){

        bpf_printk("xsk: redirect\n");

        return action;
    }

    return action;
}



char _license[] SEC("license") = "GPL";