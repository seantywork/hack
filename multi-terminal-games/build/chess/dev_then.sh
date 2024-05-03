#!/bin/bash


rm -r chess_client chess_server

rm -r dev_then

mkdir -p dev_then

cd dev_then

cmake   -DCMAKE_PREFIX_PATH=$MY_GRPC_DIR \
        -DCMAKE_BUILD_TYPE=Debug \
        -DBUILD_DEV_THEN=y \
        ..

make -j 4

cd ../

cp dev_then/chess_client .
cp dev_then/chess_server .