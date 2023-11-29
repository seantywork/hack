FROM ubuntu:20.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt update && apt install -y ca-certificates curl gnupg lsb-release wget iptables supervisor git vim build-essential

RUN mkdir -p /workspace

RUN mkdir -p /usercontent

RUN useradd -ms /bin/bash usercontent

WORKDIR /workspace

COPY . .

CMD ["./runp"]

# CMD ["tail","-f","/dev/null"]
