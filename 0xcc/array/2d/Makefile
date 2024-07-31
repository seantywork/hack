all: run.out

	cat input.txt | OUTPUT_PATH=output.txt ./run.out 

	cat output.txt

run.out:
	g++ --std=c++14 -o run.out main.cc

clean:
	rm -r *.out 