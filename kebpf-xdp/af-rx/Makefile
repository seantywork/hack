all:

	make -C kernel

	gcc -c -o ubx.o common_user_bpf_xdp.c 

	gcc -c -o p.o common_params.c 

#	gcc -c -o l.o common_libbpf.c

	gcc -L xdp-tools//lib/libbpf/src -L xdp-tools/lib/libxdp -Wl,-rpath=xdp-tools/lib/libbpf/src -Wl,-rpath=xdp-tools/lib/libxdp  -o uxdp.out main.c ubx.o p.o -lbpf -lxdp
