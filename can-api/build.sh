#!/bin/bash


sudo rm -r build

mkdir build

cd build

cmake -DSTANDALONE_BUILD ..

sudo make install
