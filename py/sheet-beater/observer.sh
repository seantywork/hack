#!/bin/bash

if [ -f ./_success ]
then 
	rm ./_success
fi

source ../VENV/bin/activate

while true
do

    if [ ! -f ./_success ]
    then
        python3 beater.py
    fi
    sleep 3
    if [ -f ./_success ]
    then 
        echo "shutting down observer"
        rm ./_success
	    exit 0
    fi
done


