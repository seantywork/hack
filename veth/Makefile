obj-m += veth2.o
veth2-y += core.o
veth2-y += core_tb.o
veth2-y += core_ex.o
veth2-y += core_ethtool.o
veth2-y += core_netdev.o
veth2-y += core_xdp_metadata.o


all:
	make -C /lib/modules/`uname -r`/build M=`pwd` modules

clean:
	make -C /lib/modules/`uname -r`/build M=`pwd` clean