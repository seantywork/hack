
GCC_FLAGS := -std=gnu99 -g -Wall


all: main.o grep.o

	gcc $(GCC_FLAGS) -I../include -o bitgrep.out main.o grep.o


main.o:

	gcc $(GCC_FLAGS) -c -I../include -o main.o main.c


grep.o: 

	gcc $(GCC_FLAGS) -c -I../include -o grep.o grep.c

clean:
	rm -r *.o *.a *.so *.out