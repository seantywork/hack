#!/bin/bash

export MY_gRPC_DIR=$HOME/.local

mkdir -p build

cd build && cmake -DCMAKE_PREFIX_PATH=$MY_gRPC_DIR ..

make -j 4