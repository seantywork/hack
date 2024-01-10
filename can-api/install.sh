#!/bin/bash

git clone https://github.com/CANopenNode/CANopenLinux.git

cd CANopenLinux

git submodule init

git submodule update

sudo rm -r ./.git

sudo rm -r ./CANopenNode/.git
