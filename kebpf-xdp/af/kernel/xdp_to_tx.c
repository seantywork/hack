


// uncomment this to get prints at /sys/kernel/debug/tracing/trace
// #define DEBUG

#define AF_INET		2	/* Internet IP Protocol 	*/
#define ETH_ALEN    6

#include <linux/bpf.h>
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_endian.h>

#include <linux/if_ether.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/in.h>
#include <linux/string.h>

/* 
// XDP enabled TX ports for redirect map 
BPF_MAP_DEF(if_redirect) = {
    .map_type = BPF_MAP_TYPE_DEVMAP,
    .key_size = sizeof(__u32),
    .value_size = sizeof(__u32),
    .max_entries = 64,
};
BPF_MAP_ADD(if_redirect);
*/
struct {
	__uint(type, BPF_MAP_TYPE_DEVMAP);
	__type(key, __u32);
	__type(value, __u32);
	__uint(max_entries, 64);
} if_redirect SEC(".maps");


SEC("xdp_do_tx")
int xdp_sock2_prog(struct xdp_md *ctx)
{
    struct bpf_fib_lookup fib_params;
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
    ip_header = data + sizeof(*eth_header);
    if ((void *)ip_header + sizeof(*ip_header) > data_end) {

        bpf_printk("packet len too short: pass\n");

        return XDP_PASS;
    }

    
    if (ip_header->protocol == IPPROTO_TCP) {

        bpf_printk("proto: tcp: drop\n");
        

        return XDP_DROP;


    } else if (ip_header->protocol == IPPROTO_UDP) { 

        bpf_printk("proto: udp: tx\n");

        tcp_header = data + sizeof(*eth_header) + sizeof(*ip_header);

        if ((void *)tcp_header + sizeof(*tcp_header) > data_end) {

            bpf_printk("packet len too short for tcp: pass\n");

            return XDP_PASS;

        }

        
        // fill struct with zeroes, so we are sure no data is missing
        __builtin_memset(&fib_params, 0, sizeof(fib_params));

        fib_params.family	= AF_INET;
        // use daddr as source in the lookup, so we refleect packet back (as if it wcame from us)
        fib_params.ipv4_src	= ip_header->saddr;
        // opposite here, the destination is the source of the icmp packet..remote end
        fib_params.ipv4_dst	= ip_header->daddr;
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
        bpf_printk("mac to use as dst: %lu\n", fib_params.dmac);

        //int action = bpf_redirect_map(&if_redirect, fib_params.ifindex, 0);

        bpf_printk("do tx\n");

        return XDP_TX;
    }



    return XDP_PASS;

}

char _license[] SEC("license") = "GPL";