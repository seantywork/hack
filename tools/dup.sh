#!/bin/bash

set -e

if (( $EUID == 0 )); then
    echo "Do not run as root"
    exit 0
fi

echo "Dependency UPdator"


mkdir -p C

mkdir -p ../bin/algorithms


git -C ./C init


git -C ./C pull https://github.com/seantywork/C.git master

sudo rm -r ./C/.git

/bin/cp -Rf ./C/* ../bin/algorithms

sudo rm -r C

echo "Successfully updated"