CC=gcc
CFLAGS=-Wall -g -fpic
LIBS=-lz
PREFIX=/usr
H_DIR=$(PREFIX)/include
L_DIR=$(PREFIX)/lib
SRCS=$(wildcard *.c)
OBJS=$(patsubst %.c, %.o, $(SRCS))

all: libpbwt.so

libpbwt.so: $(OBJS)
	gcc -shared -o $@ $^ $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

install:
	cp libpbwt.* $(L_DIR)
	cp pbwt.h $(H_DIR)

clean:
	rm $(OBJS) libpbwt.so
