FROM debian:latest

RUN apt-get update

RUN apt-get install -y gcc make zlib1g-dev

RUN mkdir /libpbwt

ADD . /libpbwt

WORKDIR /libpbwt

RUN make && make install && make -C test

CMD ["test/ptest", "test/3881.pbwt", "101"]
