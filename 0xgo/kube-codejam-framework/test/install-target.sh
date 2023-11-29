#!/bin/bash

set -euxo pipefail

if [ "$EUID" -ne 0 ]
then 
    echo "run as root"
    exit 1
fi

if [ -z "$1" ]
then
    echo "feed target"
    exit 1
fi

if [ -z "$2" ]
then

    echo "feed target address"
    exit 1
fi


OS="xUbuntu_20.04"
VERSION="1.25"

if [ "$1" = "master" ]
then

    ./installer/master.sh "$2" "$OS" "$VERSION"




elif [ "$1" = "worker" ]
then

    ./installer/worker.sh "$2" "$OS" "$VERSION"


else

    echo "wrong target"
    exit 1
fi