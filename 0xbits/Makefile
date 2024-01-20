
GCC_FLAGS := -std=c99 -g -Wall

TARGET_LIBS :=

TARGET_LIBS += maxarea-lib
TARGET_LIBS += struct-copy-thread-lib
TARGET_LIBS += arr-as-arg-lib




all:
	@echo "specify target"

all-lib:

	@echo "lib every target"

#   TODO


.PHONY: maxarea
maxarea:

	test $(BT)


	gcc $(GCC_FLAGS) -c -I. $(BT).o $(BT)/$(BT).c

	gcc $(GCC_FLAGS) -c -I. interface.o $(BT)/interface.c

	gcc $(GCC_FLAGS) -DTARGET_MAXAREA=y -I. -o main.out mainc.c $(BT).o interface.o

	cat $(BT)/input | ./main.out


maxarea-lib:

	test $(BT)

	gcc $(GCC_FLAGS) -c $(BT).o $(BT)/$(BT).c

	gcc $(GCC_FLAGS) -c -fPIC $(BT)d.o $(BT)/$(BT).c

	ar rcs lib$(BT).a $(BT).o

	gcc -shared -o lib$(BT)d.so $(BT).o


.PHONY: struct-copy-thread
struct-copy-thread:

	test $(BT)

	gcc $(GCC_FLAGS) -c -I. $(BT).o $(BT)/$(BT).c

	gcc $(GCC_FLAGS) -c -I. interface.o $(BT)/interface.c

	gcc $(GCC_FLAGS) -DTARGET_STRUCT_COPY_THREAD=y -I. -o main.out maind.c $(BT).o interface.o

	./main.out



struct-copy-thread-lib:

	test $(BT)

	gcc $(GCC_FLAGS) -c -I. $(BT).o $(BT)/$(BT).c

	gcc $(GCC_FLAGS) -c -fPIC $(BT)d.o $(BT)/$(BT).c

	ar rcs lib$(BT).a $(BT).o

	gcc -shared -o lib$(BT)d.so $(BT).o



.PHONY: arr-as-arg
arr-as-arg:

	test $(BT)

	gcc $(GCC_FLAGS) -c -I. $(BT).o $(BT)/$(BT).c

	gcc $(GCC_FLAGS) -c -I. interface.o $(BT)/interface.c

	gcc $(GCC_FLAGS) -DTARGET_ARR_AS_ARG=y -I. -o main.out maind.c $(BT).o interface.o

	./main.out


arr-as-arg-lib:

	test $(BT)

	gcc $(GCC_FLAGS) -c -I. $(BT).o $(BT)/$(BT).c

	gcc $(GCC_FLAGS) -c -fPIC $(BT)d.o $(BT)/$(BT).c

	ar rcs lib$(BT).a $(BT).o

	gcc -shared -o lib$(BT)d.so $(BT).o


clean:
	rm -r *.txt *.o *.a *.so *.out