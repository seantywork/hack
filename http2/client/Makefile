

GCC_FLAGS := -Wall -g

GCC_OBJ_FLAGS := -Wall -c

INCLUDES := -I. -I../nghttp2/lib/includes -I../nghttp2/third-party

LIBS := -lnghttp2 -lssl -lcrypto -levent -levent_openssl -lurl-parser

LINKS := -L../nghttp2/lib/.libs -L../nghttp2/third-party/.libs

LOADS := -Wl,-rpath=../nghttp2/lib/.libs


build:

	gcc $(GCC_OBJ_FLAGS) $(INCLUDES) -o http2_client.o http2_client.c 

	gcc $(GCC_FLAGS) $(INCLUDES) $(LINKS) $(LOADS) -o client.out main.c http2_client.o $(LIBS)

clean:

	rm -r *.o *.out