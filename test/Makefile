CC=gcc
CFLAGS=-Wall -g
LIBS=-lpbwt -lhts

all: ptest2

ptest2: test2.c
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

clean:
	rm ptest2
