FROM ubuntu:latest
MAINTAINER Téo Lemane teo.lemane@inria.fr

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get -y dist-upgrade \
    && apt-get install -y --no-install-recommends && apt-get clean

RUN apt-get install -y git cmake gcc g++

RUN cd /opt \
    && git clone --recursive https://github.com/tlemane/mini-miner \
    && cd mini-miner \
    && ./install.sh

RUN cd /opt/mini-miner e && chmod +x ./bin/*

WORKDIR /tmp

ENTRYPOINT ["/opt/cpp-app-template/bin/mini-miner"]
