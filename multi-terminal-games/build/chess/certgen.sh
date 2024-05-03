#!/bin/bash



rm -r ./*.pem

rm -r ./*.key

rm -r ./*.pub

rm -r ./*.csr

echo "root generating...."

openssl genrsa -out ./ca_priv.pem 4096

openssl rsa -in ./ca_priv.pem -outform PEM -pubout -out ./ca_pub.pem

openssl req -x509 -new -key ./ca_priv.pem -days 3650 -out ./ca.pem -subj "/CN=root"

echo "server key pair, csr generating...."

openssl genrsa -out ./server.key 4096

openssl rsa -in ./server.key -outform PEM -pubout -out ./server.pub

openssl req -key ./server.key -new -sha256 -out ./server.csr  -subj "/CN=localhost" 

echo "signing requests for server...."

openssl x509 -req -extfile <(printf "subjectAltName = DNS:localhost") -days 365 -in ./server.csr -CA ./ca.pem -CAkey ./ca_priv.pem -CAcreateserial -sha256 -out ./server.pem 


echo "done!"