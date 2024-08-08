all: run.out

	cat input.txt | ./run.out 


run.out: common.o

	g++ --std=c++14 -I../../ -o run.out main.cc common.o

common.o:

	gcc -c -I../../ -o common.o ../../common.c

clean:
	rm -r *.out *.o