# Copyright 2020 Darius Neatu <neatudarius@gmail.com>

# compiler setup
CC=gcc
CFLAGS=-Wall -Wextra -std=c99

SRCS=$(wildcard *.c)
OBJS=$(SRCS:%.c=%.o)
TARGETS=$(OBJS:%.o=%)

build: $(OBJS) $(TARGETS)

pack:
	zip -FSr 3XYCA_FirstnameLastname_Tema1.zip README Makefile *.c *.h

clean:
	rm -f $(TARGETS) $(OBJS)

.PHONY: pack clean
