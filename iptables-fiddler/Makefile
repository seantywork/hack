all: server client

clean:
	@rm -rf *.o
	@rm -rf *.out

fiddle: main.o httpd.o
	gcc -o fiddle.out $^

main.o: main.c httpd.h
	gcc -c -o main.o main.c

httpd.o: httpd.c httpd.h
	gcc -c -o httpd.o httpd.c
