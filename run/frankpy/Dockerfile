FROM python:3.10

ENV PYTHONDONTWRITEBYTECODE 1

ENV PYTHONUNBUFFERED 1

RUN mkdir -p /home/frank

WORKDIR /home/frank

RUN apt-get update

RUN apt-get install -y build-essential

RUN apt-get install -y pkg-config

RUN apt-get install -y default-libmysqlclient-dev 

RUN apt-get install -y python3-dev

RUN apt-get install -y libssl-dev 

COPY . .

RUN pip3 install -r requirements.txt


CMD ["gunicorn","-c","wsgi_config.py", "wsgi:app"]