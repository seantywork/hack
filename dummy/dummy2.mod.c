#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/elfnote-lto.h>
#include <linux/export-internal.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

#ifdef CONFIG_UNWINDER_ORC
#include <asm/orc_header.h>
ORC_HEADER;
#endif

BUILD_SALT;
BUILD_LTO_INFO;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif



static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0xc9755200, "dev_lstats_read" },
	{ 0x6c4daa50, "eth_mac_addr" },
	{ 0xae270153, "skb_tstamp_tx" },
	{ 0x9d007b90, "consume_skb" },
	{ 0x53569707, "this_cpu_off" },
	{ 0x2ad9428c, "eth_validate_addr" },
	{ 0x96b419c5, "ether_setup" },
	{ 0xbdfb6dbb, "__fentry__" },
	{ 0x75e73bc0, "dev_addr_mod" },
	{ 0xec66db17, "alloc_netdev_mqs" },
	{ 0xf0fdf6cb, "__stack_chk_fail" },
	{ 0x87a21cb3, "__ubsan_handle_out_of_bounds" },
	{ 0xaf793668, "__alloc_percpu_gfp" },
	{ 0x1c7f980a, "rtnl_link_unregister" },
	{ 0xfb51f98d, "free_netdev" },
	{ 0x53a1e8d9, "_find_next_bit" },
	{ 0x9e683f75, "__cpu_possible_mask" },
	{ 0x5b8239ca, "__x86_return_thunk" },
	{ 0x17de3d5, "nr_cpu_ids" },
	{ 0xc9ec4e21, "free_percpu" },
	{ 0x51d5e3db, "netif_carrier_off" },
	{ 0x3ba6c118, "netif_carrier_on" },
	{ 0x41ed3709, "get_random_bytes" },
	{ 0x3a0a8b87, "param_ops_int" },
	{ 0xb05c0b08, "skb_clone_tx_timestamp" },
	{ 0x5508f188, "ethtool_op_get_ts_info" },
	{ 0x571f0484, "register_netdevice" },
	{ 0x2fa5cadd, "module_layout" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "9D3C5483A5BE662C1D6D8FD");
