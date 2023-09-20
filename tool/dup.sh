#!/bin/bash


git clone https://github.com/curl/curl.git

sudo rm -r ./curl/.git

/bin/cp -Rf ./curl/* ../curl/

sudo rm -r ./curl
