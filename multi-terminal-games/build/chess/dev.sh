#!/bin/bash

rm -r dev 

mkdir -p dev

cd dev

cmake   -DCMAKE_PREFIX_PATH=$MY_GRPC_DIR \
        -DCMAKE_BUILD_TYPE=Debug \
        -DBUILD_DEV=y \
        ..

make -j 4

cd ../

./certgen.sh