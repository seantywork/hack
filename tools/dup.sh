#!/bin/bash

set -e

if (( $EUID == 0 )); then
    echo "Do not run as root"
    exit 0
fi

echo "Dependency UPdator"


mkdir -p yaml

mkdir -p json

mkdir -p ../cc/jsonyaml/yaml

mkdir -p ../cc/jsonyaml/json


git -C ./yaml init

git -C ./json init


git -C ./yaml pull https://github.com/jimmiebergmann/mini-yaml.git master

git -C ./json pull https://github.com/nlohmann/json.git develop


/bin/cp -Rf ./yaml/yaml/* ../cc/jsonyaml/yaml/


/bin/cp -Rf ./json/single_include/nlohmann/* ../cc/jsonyaml/json/


sudo rm -r ./yaml

sudo rm -r ./json

echo "Successfully updated"