
all: main.out

	cat in | ./main.out


main.out: s.o

	g++ -o main.out main.cc s.o

s.o: s.cc

	g++ -c -o s.o s.cc

test:
	g++ -o test.out test.cc