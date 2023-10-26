#!/bin/bash

sudo apt install -y cmake build-essential autoconf libtool pkg-config

git clone --recurse-submodules -b v1.58.0 --depth 1 --shallow-submodules https://github.com/grpc/grpc

cd grpc 

pushd cmake/build

cmake -DgRPC_INSTALL=ON \
      -DgRPC_BUILD_TESTS=OFF \
      -DCMAKE_INSTALL_PREFIX=$MY_INSTALL_DIR \

make -j 4

make install

popd