

GCC_FLAGS := -Wall -g

GCC_OBJ_FLAGS := -Wall -c

INCLUDES := -I. -I../nghttp2/lib/includes 

LIBS := -lnghttp2 -lssl -lcrypto -levent -levent_openssl

LINKS := -L../nghttp2/lib/.libs

LOADS := -Wl,-rpath=../nghttp2/lib/.libs


build:

	gcc $(GCC_OBJ_FLAGS) $(INCLUDES) -o http2_server.o http2_server.c 

	gcc $(GCC_FLAGS) $(INCLUDES) $(LINKS) $(LOADS) -o server.out main.c http2_server.o $(LIBS) 

clean:

	rm -r *.o *.out