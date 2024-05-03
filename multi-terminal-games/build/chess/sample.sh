#!/bin/bash

mkdir -p sample

cd sample

cmake -DCMAKE_PREFIX_PATH=$MY_GRPC_DIR \
      -DCMAKE_BUILD_TYPE=Debug \
      -DBUILD_SAMPLE=y \
      ..

make -j 4