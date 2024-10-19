#!/bin/bash



git clone https://github.com/xdp-project/xdp-tools


sudo apt install clang llvm libelf-dev libpcap-dev build-essential libc6-dev-i386 m4

sudo apt install linux-tools-$(uname -r)


sudo apt install linux-headers-$(uname -r)

pushd xdp-tools

./configure

make

sudo make install 

popd 

pushd xdp-tools/lib/libbpf/src 

sudo make install 

popd 