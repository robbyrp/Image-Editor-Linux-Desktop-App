# Copyright 2020 Darius Neatu <neatudarius@gmail.com>

# compiler setup
CC=gcc
CFLAGS=-Wall -Wextra -std=c99

# define targets
TARGETS=ninel codeinvim vectsecv nomogram

build: $(TARGETS)

ninel: ninel.c
	$(CC) $(CFLAGS) ninel.c -o ninel

codeinvim: codeinvim.c
	$(CC) $(CFLAGS) codeinvim.c -o codeinvim

vectsecv: vectsecv.c
	$(CC) $(CFLAGS) vectsecv.c -o vectsecv

nomogram: nomogram.c
	$(CC) $(CFLAGS) nomogram.c -o nomogram

pack:
	zip -FSr 3XYCA_FirstnameLastname_Tema1.zip README Makefile *.c *.h

clean:
	rm -f $(TARGETS)

.PHONY: pack clean
