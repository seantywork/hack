all: server

debug: server-debug

clean:
	@rm -rf *.o
	@rm -rf *.out

server: main.o httpd.o
	gcc -o server.out $^

server-debug: main.o httpd.o
	gcc -g -o server.out $^

main.o: main.c httpd.h
	gcc -c -o main.o main.c

httpd.o: httpd.c httpd.h
	gcc -c -o httpd.o httpd.c
