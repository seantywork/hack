#!/bin/bash


SPECTRE="$1"

if [ -z "$SPECTRE" ]
then 
    echo "set path for spectre"
    exit 1
fi

/bin/cp -Rf ./* "$SPECTRE" 