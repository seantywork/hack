FROM ubuntu:20.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt update && apt install -y build-essential

RUN mkdir -p /workspace

RUN mkdir -p /usercontent

RUN useradd -ms /bin/bash usercontent

WORKDIR /workspace

COPY . .

CMD ["./runp"]

# CMD ["tail","-f","/dev/null"]
