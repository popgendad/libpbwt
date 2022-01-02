FROM debian:latest
RUN apt-get update
RUN apt-get install -y gcc make libhts-dev
RUN mkdir -p /usr/local/src/libpbwt
COPY ./ /usr/local/src/libpbwt/
WORKDIR /usr/local/src/libpbwt
RUN cd libplink_lite && make && make install
RUN make && make install
