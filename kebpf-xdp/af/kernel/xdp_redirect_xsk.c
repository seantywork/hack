#include <linux/bpf.h>

#include <bpf/bpf_helpers.h>

#include <linux/if_ether.h>
#include <linux/ip.h>
#include <linux/tcp.h>
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
    

    void *data_end = (void *)(long)ctx->data_end;
    void *data = (void *)(long)ctx->data;
    struct ethhdr *eth_header;
    struct iphdr *ip_header;
    struct tcphdr *tcp_header;
    eth_header = data;

    if ((void *)eth_header + sizeof(*eth_header) > data_end) {
        return XDP_PASS;
    }

    __u16 h_proto = eth_header->h_proto;

    /* anything that is not IPv4 (including ARP) goes up to the kernel */
    if (h_proto != 0x08U) {  // htons(ETH_P_IP) -> 0x08U
        return XDP_PASS;
    }

    int* val = bpf_map_lookup_elem(&if_redirect, &index);

    if (val){

        bpf_printk("val read from the map\n");

    } else {

        bpf_printk("failed to read val from the map\n");

        return XDP_ABORTED;

    }

    int action = bpf_redirect_map(&if_redirect, index, 0);

    if (action == XDP_ABORTED){

        bpf_printk("aborted\n");
        return action;
    }

    if(action == XDP_REDIRECT){

        bpf_printk("redirect\n");

        return action;
    }

    return action;
}

char _license[] SEC("license") = "GPL";
