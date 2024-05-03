#!/bin/bash

mkdir -p sample_then

cd sample_then

cmake -DCMAKE_PREFIX_PATH=$MY_GRPC_DIR \
      -DCMAKE_BUILD_TYPE=Debug \
      -DBUILD_SAMPLE_THEN=y \
      ..

make -j 4