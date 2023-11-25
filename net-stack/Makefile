all: recv send

debug: recv-debug send-debug

clean:
	@rm -rf *.o
	@rm -rf *.out

recv: main.o recv.o
	gcc -o recv.out $^

recv-debug: main.o recv.o
	gcc -g -o recv.out $^

main.o: main.c recv.h
	gcc -c -o main.o main.c

recv.o: recv.c recv.h
	gcc -c -o recv.o recv.c

send: send.c
	gcc -o send.out send.c 

send-debug: send.c
	gcc -g -o send.out send.c 