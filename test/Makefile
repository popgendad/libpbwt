CC=gcc
CFLAGS=-Wall -O2
LIBS=-lpbwt

all: ptest

ptest: test.c
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

clean:
	rm ptest
