#!/bin/bash

DEP_ROSCANCOMM="hardware/roscancomm"

if [ -d "hardware/roscancomm/CANopenLinux" ]
then
    sudo rm -r "hardware/roscancomm/CANopenLinux"
fi

pushd "$DEP_ROSCANCOMM"

git clone https://github.com/CANopenNode/CANopenLinux.git

cd CANopenLinux

git submodule init

git submodule update

sudo rm -r ./.git

sudo rm -r ./CANopenNode/.git

popd


sudo apt-get install -y can-utils


