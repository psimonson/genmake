CC = gcc
CFLAGS = -std=c11 -Wall -Werror -D_DEFAULT_SOURCE -O3
LDFLAGS = 

SRCDIR = $(shell basename $(shell pwd))
DESTDIR ?= 
PREFIX ?= /usr

SRC0 =  main.c
OBJ0 = $(SRC0:%.c=%.c.o)
EXE0 = genmake

all: $(EXE0)

$(EXE0): $(OBJ0)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.c.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJ0) $(EXE0)

install:
	cp $(EXE0) $(DESTDIR)$(PREFIX)/bin

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/$(EXE0)

dist:
	cd .. && tar cvzf $(SRCDIR).tgz ./$(SRCDIR)

