
GCC_FLAGS := -std=gnu99 -g -Wall


all: main.o cat.o

	gcc $(GCC_FLAGS) -I../include -o bitcat.out main.o cat.o


main.o:

	gcc $(GCC_FLAGS) -c -I../include -o main.o main.c


cat.o: 

	gcc $(GCC_FLAGS) -c -I../include -o cat.o cat.c

clean:
	rm -r *.o *.a *.so *.out