
GCC_FLAGS := -std=c99 -g -Wall


all:
	@echo "specify target"


.PHONY: maxarea
maxarea:

	gcc $(GCC_FLAGS) -c -I. maxarea.o maxarea/maxarea.c

	gcc $(GCC_FLAGS) -c -I. interface.o maxarea/interface.c

	gcc $(GCC_FLAGS) -DTARGET_MAXAREA=y -I. -o main.out mainc.c maxarea.o interface.o

	cat maxarea/input | ./main.out


.PHONY: longest-palindromic-substr
longest-palindromic-substr:

	gcc $(GCC_FLAGS) -c -I. longest-palindromic-substr.o longest-palindromic-substr/longest-palindromic-substr.c

	gcc $(GCC_FLAGS) -c -I. interface.o longest-palindromic-substr/interface.c

	gcc $(GCC_FLAGS) -DTARGET_LONGEST_PALINDROMIC_SUBSTR=y -I. -o main.out mainc.c longest-palindromic-substr.o interface.o

	cat longest-palindromic-substr/input | ./main.out
