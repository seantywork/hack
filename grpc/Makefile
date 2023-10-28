all:
	@echo "gRPC"


go-protoc:

	protoc 	--go_out=. --go_opt=paths=source_relative \
    		--go-grpc_out=. --go-grpc_opt=paths=source_relative \
		    go/protobuf/svc.proto

go-proto-build:

	go build -o go/protoclient/client.out ./go/protoclient 

	go build -o go/protoserver/server.out ./go/protoserver

go-flatc:

	flatc 	--go --grpc go/flatbuf/svc.fbs

	mv bookmarks go/flatbuf


go-flat-build:

	go build -o go/flatclient/client.out ./go/flatclient 

	go build -o go/flatserver/server.out ./go/flatserver


go-example:
	git clone -b v1.58.2 --depth 1 https://github.com/grpc/grpc-go

