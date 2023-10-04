FROM ubuntu:20.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt update && apt install -y ca-certificates curl gnupg lsb-release wget iptables supervisor git vim

RUN apt install -y python3-pip

RUN pip3 install gunicorn

RUN pip3 install flask

RUN pip3 install requests

RUN pip3 install Flask-Session

RUN pip3 install mysql-connector-python

CMD ["tail", "-f", "/dev/null"]