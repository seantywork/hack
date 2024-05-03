#!/bin/bash

export MY_GRPC_DIR=$HOME/.grpc

mkdir -p $MY_GRPC_DIR

export PATH="$MY_GRPC_DIR/bin:$PATH"

git clone --recurse-submodules -b v1.60.0 --depth 1 --shallow-submodules https://github.com/grpc/grpc

cd grpc

mkdir -p cmake/build

pushd cmake/build

cmake -DgRPC_INSTALL=ON \
      -DgRPC_BUILD_TESTS=OFF \
      -DCMAKE_INSTALL_PREFIX=$MY_GRPC_DIR \
      ../..

make -j 4

sudo make install

popd