#!/bin/bash

set -euxo pipefail

TARGET_IMAGE="$1"

TARGET_REG="$2"

go build -o ./runo ../../../jam

echo "jam copied to current directory"

sudo docker-compose up --build -d

sudo ocker-compose down

sudo docker tag "$TARGET_IMAGE" "$TARGET_REG"

sudo docker push "$TARGET_REG"


echo "jam image updated and pushed"

rm -r runo