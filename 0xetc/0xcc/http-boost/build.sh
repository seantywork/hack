#!/bin/bash

rm -rf build

mkdir -p build


pushd build

cmake ..

make

mv main.out ..

popd

rm -rf build