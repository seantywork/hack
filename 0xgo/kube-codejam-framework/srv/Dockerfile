FROM ubuntu:20.04

RUN apt-get update

RUN apt-get install -y ca-certificates curl

RUN mkdir /run-payload-srv

WORKDIR /run-payload-srv

COPY . .


CMD ["/run-payload-srv/runo"]