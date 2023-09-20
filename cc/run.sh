#!/bin/bash


MK_TARGET=""

if [ -z "$1" ]
then
	echo "feed target"
	exit 0
fi

MK_TARGET="$1"

make "$MK_TARGET"

cat "$MK_TARGET/tc" | "./$MK_TARGET/main.out"
