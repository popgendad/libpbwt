CC=gcc
CFLAGS=-Wall -O2 -I. -I./libplink
LIBS=-lz
BIN=plink2pbwt

all: plink2pbwt

plink2pbwt: plink2pbwt.c libplink.a libpbwt.a
	$(CC) $(CFLAGS) -o $@ $< libplink/libplink.a libpbwt.a $(LIBS)

libplink.a:
	make -C libplink/ all

libpbwt.a: pbwt.o
	ar rcs $@ $<
	ranlib $@

pbwt.o: pbwt.c
	$(CC) -Wall -O2 -fPIC -c $< $(LIBS)

clean:
	rm *.o libpbwt.a $(BIN)
	make -C libplink clean
