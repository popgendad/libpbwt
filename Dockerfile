FROM debian:latest

RUN apt-get update

RUN apt-get install -y gcc make zlib1g-dev

RUN mkdir /libpbwt

ADD . /libpbwt

WORKDIR /libpbwt

RUN make && make install

CMD ["/bin/bash"]
