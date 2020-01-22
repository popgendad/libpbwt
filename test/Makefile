CC=gcc
CFLAGS=-Wall -g
LIBS=-lpbwt -lhts

all: ptest

ptest: test.c
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

clean:
	rm ptest
