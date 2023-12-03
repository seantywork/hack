#!/bin/bash


WORKDIR=$1

if [ -f "$WORKDIR/sheet-beater/_success" ]
then 
	rm "$WORKDIR/sheet-beater/_success"
fi

source "$WORKDIR/VENV/bin/activate"

while true
do

    if [ ! -f "$WORKDIR/sheet-beater/_success" ]
    then
        python3 "$WORKDIR/sheet-beater/beater.py" "$WORKDIR/sheet-beater"
    fi
    sleep 3
    if [ -f "$WORKDIR/sheet-beater/_success" ]
    then 
        echo "shutting down observer"
        rm "$WORKDIR/sheet-beater/_success"
	exit 0
    fi
done


