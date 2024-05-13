

DEP_PACKAGES := libssl-dev libevent-dev zlib1g-dev libxml2-dev libev-dev 


NGHTTP2_VERSION := 1.61.0


all:

	@echo "http2"


dep-package:

	apt install -y $(DEP_PACKAGES)

dep-source:

	wget https://github.com/nghttp2/nghttp2/releases/download/v$(NGHTTP2_VERSION)/nghttp2-$(NGHTTP2_VERSION).tar.gz

	tar -xzf nghttp2-$(NGHTTP2_VERSION).tar.gz

	mv nghttp2-$(NGHTTP2_VERSION) nghttp2

	cd nghttp2 && ./configure && make


build:

	make -C server build 

	make -C client build


clean:

	make -C server clean 

	make -C client clean