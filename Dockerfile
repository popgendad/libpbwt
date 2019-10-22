FROM 552622921787.dkr.ecr.us-east-1.amazonaws.com/dnascience/amazonlinux2:latest

RUN yum --disablerepo=amzn2-core makecache

RUN yum install --disablerepo=amzn2-core -y gcc make nano

RUN mkdir /libpbwt

ADD . /libpbwt

WORKDIR /libpbwt

RUN make && make install && ldconfig

RUN make -C test

CMD [ "test/ptest", "test/EV62-phase.3881.pbwt", "100" ]



