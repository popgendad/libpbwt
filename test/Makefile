CC=gcc
CFLAGS=-Wall -g
LIBS=-lpbwt

all: ptest

ptest: test.c
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

clean:
	rm ptest
