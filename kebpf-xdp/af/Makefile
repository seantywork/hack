all:

	gcc -c -o ubx.o common_user_bpf_xdp.c 

	gcc -c -o p.o common_params.c 

#	gcc -c -o l.o common_libbpf.c

	gcc -L ./libbpf/src -L xdp-tools/lib/libxdp -Wl,-rpath=./libbpf/src -Wl,-rpath=xdp-tools/lib/libxdp  -o uxdp.out main.c p.o -lbpf -lxdp
