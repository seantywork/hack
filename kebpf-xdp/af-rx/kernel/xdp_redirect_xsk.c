
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
	__uint(max_entries, 1);
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

        

    } else if (ip_header->protocol == IPPROTO_TCP){

        bpf_printk("xsk: proto: tcp\n");

        
        tcp_header = data + sizeof(*eth_header) + sizeof(*ip_header);
        

    } else if (ip_header->protocol == IPPROTO_UDP){

        bpf_printk("xsk: proto: udp\n");
        
        
        udp_header = data + sizeof(*eth_header) + sizeof(*ip_header);
            
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