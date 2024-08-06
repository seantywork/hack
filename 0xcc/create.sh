#!/bin/bash


SUBJ="$1"
TITLE="$2"

if [ -z "$SUBJ" ]
then

    echo "needs SUBJ at 1"
    exit 1
fi

if [ -z "$TITLE" ]
then

    echo "needs TITLE at 2"
    exit 1
fi


SHOW="$SUBJ/$TITLE"

mkdir -p "$SHOW"


/bin/cp -Rf Makefile.tmpl "$SHOW/Makefile"

/bin/cp -Rf main.cc.tmpl "$SHOW/main.cc"

echo "" | tee "$SHOW/input.txt"

echo "$SHOW created"