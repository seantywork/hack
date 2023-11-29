
all: test.out

	cat in | ./test.out


test.out: algo.o

	g++ -o test.out test.cc algo.o

algo.o: algo.cc

	g++ -c -o algo.o algo.cc

clean:

	rm -r *.out *.o
