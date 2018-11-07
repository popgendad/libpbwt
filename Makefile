CC=gcc
CFLAGS=-Wall -O2 -fPIC

all: libpbwt.a

libpbwt.a: pbwt.o
	ar rcs $@ $<
	ranlib $@

pbwt.o: pbwt.c
	$(CC) $(CFLAGS) -c $< $(LIBS)

clean:
	rm pbwt.o libpbwt.a
