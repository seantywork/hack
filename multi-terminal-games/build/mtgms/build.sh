#!/bin/bash

mkdir -p build

cd build && cmake -DCMAKE_PREFIX_PATH=$MY_GRPC_DIR ..

make -j 4