
GCC_FLAGS := -std=c99 -g -Wall




all:
	@echo "specify target"



.PHONY: struct-copy-thread
struct-copy-thread:

	gcc $(GCC_FLAGS) -c -I. struct-copy-thread.o struct-copy-thread/struct-copy-thread.c

	gcc $(GCC_FLAGS) -c -I. interface.o struct-copy-thread/interface.c

	gcc $(GCC_FLAGS) -DTARGET_STRUCT_COPY_THREAD=y -I. -o main.out maind.c struct-copy-thread.o interface.o

	./main.out



.PHONY: arr-as-arg
arr-as-arg:


	gcc $(GCC_FLAGS) -c -I. arr-as-arg.o arr-as-arg/arr-as-arg.c

	gcc $(GCC_FLAGS) -c -I. interface.o arr-as-arg/interface.c

	gcc $(GCC_FLAGS) -DTARGET_ARR_AS_ARG=y -I. -o main.out maind.c arr-as-arg.o interface.o

	./main.out


.PHONY: stream-to-2d
stream-to-2d:

	gcc $(GCC_FLAGS) -c -I. stream-to-2d.o stream-to-2d/stream-to-2d.c

	gcc $(GCC_FLAGS) -c -I. interface.o stream-to-2d/interface.c

	gcc $(GCC_FLAGS) -DTARGET_STREAM_TO_2D=y -I. -o main.out maind.c stream-to-2d.o interface.o

	cp stream-to-2d/test.txt . && ./main.out

clean:
	rm -r *.txt *.o *.a *.so *.out