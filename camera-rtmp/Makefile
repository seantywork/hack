

all:

	libtool --mode=link gcc `pkg-config --cflags --libs gstreamer-1.0` -o camrtmp.out main.c

clean:

	rm -r *.out *.webm
