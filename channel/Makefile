all:

	gcc -c -I./chan/src -o chan.o ./chan/src/chan.c 
	
	gcc -c -I./chan/src -o queue.o ./chan/src/queue.c

	gcc -I./chan/src -o test.out main.c chan.o queue.o -lpthread

vendor:

	git clone https://github.com/seantywork/chan.git

clean:

	rm -rf *.o *.out