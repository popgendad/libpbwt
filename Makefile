CC      := gcc
VERSION := $(shell cat VERSION)
UNAME      := $(shell uname)
CFLAGS  := -Wall -O2 -D VERSION=$(VERSION) -I./libplink_lite/src
ifeq ($(UNAME), Darwin)
CFLAGS  := -Wall -O2 -D VERSION=$(VERSION) -I/opt/homebrew/Cellar/htslib/1.14/include -L/opt/homebrew/Cellar/htslib/1.14/lib
endif
LIBS    := -lm -lz -lplink_lite -lhts
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
	cp libpbwt.* /usr/local/lib/
	cp src/pbwt.h /usr/local/include/pbwt.h

clean:
	rm -rf $(DDIR) $(SDIR) libpbwt.*
