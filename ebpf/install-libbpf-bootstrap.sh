#!/bin/bash

sudo apt install clang libelf1 libelf-dev zlib1g-dev

git clone --recurse-submodules https://github.com/libbpf/libbpf-bootstrap

cd libbpf-bootstrap 

git submodule update --init --recursive   
