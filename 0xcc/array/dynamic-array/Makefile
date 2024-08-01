all: run.out

	cat input.txt | OUTPUT_PATH=output.txt ./run.out 

	cat output.txt

run.out: common.o

	g++ --std=c++14 -I../../ -o run.out main.cc common.o

common.o:

	gcc -c -I../../ -o common.o ../../common.c

clean:
	rm -r *.out *.o