CC=gcc
CFLAGS=-Wall -g -fpic
LIBS=-lz
PREFIX=/usr/local
H_DIR=$(PREFIX)/include
L_DIR=$(PREFIX)/lib

all: libpbwt.so

libpbwt.so: pbwt.o
	gcc -shared -o $@ $< $(LIBS)

pbwt.o: pbwt.c
	$(CC) $(CFLAGS) -c $<

install:
	cp libpbwt.* $(L_DIR)
	cp pbwt.h $(H_DIR)

clean:
	rm pbwt.o libpbwt.so
