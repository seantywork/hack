

all:

	@echo "gRPC"

install:

	./install.sh

protoc:

	protoc --grpc_out=./hi --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` ./hi/hi.proto

	protoc --cpp_out=./hi ./hi/hi.proto