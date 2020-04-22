CC      := gcc
VERSION := $(shell cat VERSION)
CFLAGS  := -Wall -O2 -D VERSION=$(VERSION) -I/shared/include -L/shared/lib
LIBS    := -lm -lz -lplink_lite -lhts
PREFIX  := /shared
H_DIR   := $(PREFIX)/include
L_DIR   := $(PREFIX)/lib
SRCS    := $(wildcard src/*.c)
SDIR    := build-static
DDIR    := build-dynamic
DOBJS   := $(SRCS:src/%.c=$(DDIR)/%.o)
SOBJS   := $(SRCS:src/%.c=$(SDIR)/%.o)

all: libpbwt.so libpbwt.a

static: libpbwt.a

shared: libpbwt.so

libpbwt.a: $(SOBJS)
	ar rcs $@ $^

libpbwt.so: CFLAGS+=-fPIC

libpbwt.so: $(DOBJS)
	gcc -shared $(CFLAGS) -o $@ $^ $(LIBS)

$(DDIR)/%.o:src/%.c | $(DDIR)
	$(CC) $(CFLAGS) -o $@ -c $<

$(SDIR)/%.o:src/%.c | $(SDIR)
	$(CC) $(CFLAGS) -o $@ -c $<

$(SDIR) $(DDIR):
	@mkdir $@

install:
	cp libpbwt.* $(L_DIR)
	cp src/pbwt.h $(H_DIR)

clean:
	rm -rf $(DDIR) $(SDIR) libpbwt.*
