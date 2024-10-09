all:

	gcc -c -o uxdp_libbpf.o uxdp_libbpf.c

	gcc -c -o uxdp_params.o uxdp_params.c

	gcc -c -o uxdp_user_bpf_xdp.o uxdp_user_bpf_xdp.c

	gcc -L ./libbpf/src -Wl,-rpath=./libbpf/src -o uxdp.out main.c uxdp_libbpf.o uxdp_params.o uxdp_user_bpf_xdp.o -lbpf -lxdp

clean:

	rm -rf *.o *.out