
GCC_FLAGS := -Wall

INCLUDES := -I../0xgo/c-lib

LIBS := ../0xgo/c-lib/go2c.so

LINKS := -L../0xgo/c-lib

LOADS := -Wl,-rpath=../0xgo/c-lib

all:

	gcc $(GCC_FLAGS) $(INCLUDES) $(LINKS) $(LOADS) -o go2c.out main.c $(LIBS)

clean:
	rm -r *.out