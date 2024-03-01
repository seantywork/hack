
GCC_FLAGS := -std=c99 -g -Wall


all:
	@echo "specify target"


.PHONY: maxarea
maxarea:

	gcc $(GCC_FLAGS) -c -I../include maxarea.o maxarea/maxarea.c

	gcc $(GCC_FLAGS) -c -I../include interface.o maxarea/interface.c

	gcc $(GCC_FLAGS) -DTARGET_MAXAREA=y -I../include -o main.out main.c maxarea.o interface.o

	cat maxarea/input | ./main.out


.PHONY: longest-palindromic-substr
longest-palindromic-substr:

	gcc $(GCC_FLAGS) -c -I../include longest-palindromic-substr.o longest-palindromic-substr/longest-palindromic-substr.c

	gcc $(GCC_FLAGS) -c -I../include interface.o longest-palindromic-substr/interface.c

	gcc $(GCC_FLAGS) -DTARGET_LONGEST_PALINDROMIC_SUBSTR=y -I../include -o main.out main.c longest-palindromic-substr.o interface.o

	cat longest-palindromic-substr/input | ./main.out



.PHONY: reverse-int32
reverse-int32:

	gcc $(GCC_FLAGS) -c -I../include reverse-int32.o reverse-int32/reverse-int32.c

	gcc $(GCC_FLAGS) -c -I../include interface.o reverse-int32/interface.c

	gcc $(GCC_FLAGS) -DTARGET_REVERSE_INT32=y -I../include -o main.out main.c reverse-int32.o interface.o

	cat reverse-int32/input | ./main.out




.PHONY: struct-copy-thread
struct-copy-thread:

	gcc $(GCC_FLAGS) -c -I../include struct-copy-thread.o struct-copy-thread/struct-copy-thread.c

	gcc $(GCC_FLAGS) -c -I../include interface.o struct-copy-thread/interface.c

	gcc $(GCC_FLAGS) -DTARGET_STRUCT_COPY_THREAD=y -I../include -o main.out main.c struct-copy-thread.o interface.o

	./main.out



.PHONY: arr-as-arg
arr-as-arg:


	gcc $(GCC_FLAGS) -c -I../include arr-as-arg.o arr-as-arg/arr-as-arg.c

	gcc $(GCC_FLAGS) -c -I../include interface.o arr-as-arg/interface.c

	gcc $(GCC_FLAGS) -DTARGET_ARR_AS_ARG=y -I../include -o main.out main.c arr-as-arg.o interface.o

	./main.out


.PHONY: stream-to-2d
stream-to-2d:

	gcc $(GCC_FLAGS) -c -I../include stream-to-2d.o stream-to-2d/stream-to-2d.c

	gcc $(GCC_FLAGS) -c -I../include interface.o stream-to-2d/interface.c

	gcc $(GCC_FLAGS) -DTARGET_STREAM_TO_2D=y -I../include -o main.out main.c stream-to-2d.o interface.o

	cp stream-to-2d/test.txt . && ./main.out


.PHONY: stream-to-2d-copy
stream-to-2d-copy:

	gcc $(GCC_FLAGS) -c -I../include stream-to-2d-copy.o stream-to-2d-copy/stream-to-2d-copy.c

	gcc $(GCC_FLAGS) -c -I../include interface.o stream-to-2d-copy/interface.c

	gcc $(GCC_FLAGS) -DTARGET_STREAM_TO_2D_COPY=y -I../include -o main.out main.c stream-to-2d-copy.o interface.o

	cp stream-to-2d-copy/test.txt . && ./main.out

.PHONY: stream-to-2d-struct
stream-to-2d-struct:

	gcc $(GCC_FLAGS) -c -I../include stream-to-2d-struct.o stream-to-2d-struct/stream-to-2d-struct.c

	gcc $(GCC_FLAGS) -c -I../include interface.o stream-to-2d-struct/interface.c

	gcc $(GCC_FLAGS) -DTARGET_STREAM_TO_2D_STRUCT=y -I../include -o main.out main.c stream-to-2d-struct.o interface.o

	cp stream-to-2d-struct/test.txt . && ./main.out


.PHONY: push-back-string
push-back-string:

	gcc $(GCC_FLAGS) -c -I../include push-back-string.o push-back-string/push-back-string.c

	gcc $(GCC_FLAGS) -c -I../include interface.o push-back-string/interface.c

	gcc $(GCC_FLAGS) -DTARGET_PUSH_BACK_STRING=y -I../include -o main.out main.c push-back-string.o interface.o

	cp push-back-string/test.txt . && ./main.out



clean:
	rm -r *.txt *.o *.a *.so *.out