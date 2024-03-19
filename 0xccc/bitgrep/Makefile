
GCC_FLAGS := -std=gnu99 -g -Wall


all: main.o grep.o libvector.a

	gcc $(GCC_FLAGS) -I../include -L. -o bitgrep.out main.o grep.o -lvector


main.o:

	gcc $(GCC_FLAGS) -c -I../include -o main.o main.c


grep.o: 

	gcc $(GCC_FLAGS) -c -I../include -o grep.o grep.c

libvector.a:

	gcc $(GCC_FLAGS) -c -I../include -o vec_string.o ../bit/vector/string.c 

	gcc $(GCC_FLAGS) -c -I../include -o vec_int.o ../bit/vector/int.c 

	ar rcs libvector.a vec_string.o vec_int.o

clean:
	rm -r *.o *.a *.so *.out