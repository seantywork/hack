FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

RUN mkdir /workspace

WORKDIR /workspace

COPY . .

RUN apt update
RUN apt install -y git
RUN apt install -y vim
RUN apt install -y curl
RUN apt install -y gnupg
RUN apt install -y python3-pip

RUN pip3 install -r requirements.txt


CMD ["tail","-f","/dev/null"]
