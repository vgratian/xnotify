
CC		:= cc
CFLAGS	:= -std=gnu99 -pedantic -Wall -Os
LIBS	:= -lX11 -lXft
INCS	:= $(shell pkg-config --cflags freetype2)
LOGDEF	 = $(shell grep '^#define LOGFN ' config.h)
LOGARGS	 = $(if $(LOGDEF), log.c -D LOGDIR=\"`pwd`\", )

SRC		:= main.c config.h xwindow.h xwindow.c log.h log.c

all: options xnotify

options: config.h
	@echo build options:
	@echo "	CC       = ${CC}"
	@echo "	CFLAGS   = ${CFLAGS}"
	@echo "	LIBS     = ${LIBS}"
	@echo "	INCS     = ${INCS}"
	@echo "	LOGARGS  = ${LOGARGS}"
	@echo

xnotify: $(SRC)
	${CC} ${CFLAGS} main.c xwindow.c ${LOGARGS} -o $@ ${LIBS} ${INCS}

config.h:
	cp config.def.h $@
	@echo
    
clean:
	rm -f xnotify

.PHONY: all clean options
