#!/bin/bash

sudo apt update

sudo apt install -y protobuf-compiler

sudo apt install -y flatbuffers-compiler

go install google.golang.org/protobuf/cmd/protoc-gen-go@v1.28

go install google.golang.org/grpc/cmd/protoc-gen-go-grpc@v1.2

echo 'TODO: export PATH="$PATH:$(go env GOPATH)/bin"'

