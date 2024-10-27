all:

	gcc -g -c -o af_rxtx.o af_rxtx.c

	gcc -g -L ../xdp-tools/lib/libbpf/src -L ../xdp-tools/lib/libxdp -Wl,-rpath=../xdp-tools/lib/libbpf/src -Wl,-rpath=../xdp-tools/lib/libxdp  -o run.out main.c af_rxtx.o -lbpf -lxdp


clean:

	rm -rf *.out *.o