
#define PROTO_IP     0x0800

#include <linux/bpf.h>
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_endian.h>
#include <xdp/xdp_helpers.h>

#include <linux/if_ether.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/udp.h>
#include <linux/in.h>
#include <linux/string.h>


struct ipv4_lpm_key {
        __u32 prefixlen;
        __u32 data;
};


struct {
  __uint(type, BPF_MAP_TYPE_LPM_TRIE);
  __type(key, struct ipv4_lpm_key);
  __type(value, __u32);
  __uint(map_flags, BPF_F_NO_PREALLOC);
  __uint(max_entries, 255);
} allowed_addr SEC(".maps");

struct {
	__uint(priority, 20);
	__uint(XDP_PASS, 1);
} XDP_RUN_CONFIG(allowed_addr_prog);


SEC("xdp")

int allowed_addr_prog(struct xdp_md *ctx) {

  unsigned char *data_end = (unsigned char *)(long)ctx->data_end;
  unsigned char *data = (unsigned char *)(long)ctx->data;

  struct ethhdr *eth_header = (struct ethhdr *)data;
  if (data + sizeof(*eth_header) > data_end) {

    return XDP_DROP;
  }

  struct iphdr *ip = (struct iphdr *)data;
  if (data + sizeof(*ip) > data_end) {

    return XDP_DROP;
  }



  struct {
    __u32 prefixlen;
    __u32 data;
  } key;

  __u16 allow_port = 0; 

  key.prefixlen = 32;
  key.data = 0;

  __u64 *allow_port_v = bpf_map_lookup_elem(&allowed_addr, &key);
  if (!allow_port_v) {
    bpf_printk("allow port not stored : %llp\n", allow_port_v);
    return XDP_DROP;
  }

  allow_port = *(__u16*)(allow_port_v);


  if (allow_port == 0) {

    bpf_printk("allow port still zero: drop\n");

    return XDP_DROP;
  }

  if (ip->protocol == IPPROTO_UDP) { 

    bpf_printk("proto udp \n"); 
    data += ip->ihl * 4;
    struct udphdr *udp = (struct udphdr *)data;
    if (data + sizeof(*udp) > data_end) {

      return XDP_DROP;
    }

  
    if (bpf_ntohs(udp->dest) == allow_port){
      bpf_printk("coming in allowed packet\n");
      return XDP_PASS;
    } else {
      bpf_printk("udp dest not allowed port\n");
      return XDP_PASS;
    }

  }

  bpf_printk("not udp: drop\n");

  return XDP_DROP;

}