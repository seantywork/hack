all:
	@echo "gRPC"


go-protoc:

	protoc 	--go_out=. --go_opt=paths=source_relative \
    		--go-grpc_out=. --go-grpc_opt=paths=source_relative \
		    go/protobuf/svc.proto


go-build:

	go build -o go/client/client.out ./go/client 

	go build -o go/server/server.out ./go/server


go-example:
	git clone -b v1.58.2 --depth 1 https://github.com/grpc/grpc-go

