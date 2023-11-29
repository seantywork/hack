#!/bin/bash

go build -o ../adm/runo ../adm

go build -o ../srv/runo ../srv

echo "successfully built all packages"

sudo docker-compose -f ../docker-compose.yaml up --build