all: 
	@echo "infrared"

receiver:

	gcc -o irrecv.out ir_recv_main.c  ir_recv.c -lpigpio -lrt -lpthread

transmitter:

	gcc -o irsend.out ir_send.c -lm -lpigpio -lrt -lpthread 

clean:
	rm -r *.out *.o
