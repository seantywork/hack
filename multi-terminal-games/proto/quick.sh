#!/bin/bash

protoc -I . \
       --grpc_out=. \
       --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` \
       ./chess.proto


protoc -I . \
       --cpp_out=. \
       ./chess.proto
