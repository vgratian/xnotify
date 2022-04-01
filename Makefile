
CC = cc
LIBS = -lX11 -lXft
INCS = `pkg-config --cflags freetype2`
CFLAGS = -std=gnu99 -pedantic -Wall -Os

all: options xnotify

options:
	@echo
	@echo xnotify build options:
	@echo "CC       = ${CC}"
	@echo "CFLAGS   = ${CFLAGS}"
	@echo "LIBS     = ${LIBS}"
	@echo "INCS     = ${INCS}"
	@echo

config.h:
	cp config.def.h $@
    
xnotify: config.h
	${CC} ${CFLAGS} main.c xwindow.c -o $@ ${LIBS} ${INCS}

clean:
	rm -f xnotify

.PHONY: all clean options
