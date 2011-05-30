IDIR=include

CFLAGS+=-Wall
CFLAGS+=-g
CFLAGS+=-std=gnu99
CFLAGS+=-Iinclude
CFLAGS+=-lsqlite3
CFLAGS+=-lircclient


OBJS:=$(wildcard src/*.c *.c)
OBJS:=$(OBJS:.c=.o)

src/%.o: src/%.c
	@$(CC) $(CFLAGS) -c -o $@ $<
	@echo " CC $<"

%.o: %.c %.h
	@$(CC) $(CFLAGS) -c -o $@ $<
	@echo " CC $<"

all: IRCdaemon_C manpage

clean:
	rm -f *.o src/*.o

manpage:
	make -C man

install:lib
	#-mkdir @prefix@/include
	#-mkdir @prefix@/lib
	#cp ../include/libircclient.h @prefix@/include/libircclient.h
	#cp ../include/libirc_errors.h @prefix@/include/libirc_errors.h
	#cp ../include/libirc_events.h  @prefix@/include/libirc_events.h 
	#cp libirc.a @prefix@/include/lib/libirc.a

	install -m 755 -d $(DESTDIR)/usr/bin
	install -m 755 -d $(DESTDIR)/usr/share/man/man1
	install -m 644 man/IRCdaemon_C.1 $(DESTDIR)/usr/share/man/man1
