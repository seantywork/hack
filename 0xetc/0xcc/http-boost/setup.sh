#!/bin/bash

rm -rf civetweb

rm -rf boost_*

git clone https://github.com/seantywork/civetweb.git

curl -L https://boostorg.jfrog.io/artifactory/main/release/1.87.0/source/boost_1_87_0.tar.gz -o boost_1_87_0.tar.gz

tar -xzf boost_1_87_0.tar.gz

pushd civetweb 

make clean 

make WITH_IPV6=1 WITH_WEBSOCKET=1 COPT='-DMG_EXPERIMENTAL_INTERFACES' lib 

popd

pushd boost_1_87_0

./bootstrap.sh --prefix=./build

./b2 install

popd