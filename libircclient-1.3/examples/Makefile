# $Id: Makefile.in 42 2004-10-10 16:16:15Z gyunaev $
CC = gcc
CXX = g++
CFLAGS = -Wall -O3 -DENABLE_THREADS -D_REENTRANT
LIBS = ../src/libircclient.a -lpthread  -lnsl
INCLUDES=-I../include

EXAMPLES=spammer censor irctest ircftp colors

all:	$(EXAMPLES)

spammer:	spammer.o
	$(CC) -o spammer spammer.o $(LIBS)

colors:		colors.o
	$(CXX) -o colors colors.o $(LIBS)

irctest:	irctest.o
	$(CC) -o irctest irctest.o $(LIBS)

censor:	censor.o
	$(CXX) -o censor censor.o $(LIBS)

ircftp:	ircftp.o
	$(CXX) -o ircftp ircftp.o $(LIBS)


clean:
	-rm -f $(EXAMPLES) *.o *.exe

distclean: clean
	-rm -f Makefile *.log


.c.o:
	@echo "Compiling  $<"
	@$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<

.cpp.o:
	@echo "Compiling  $<"
	@$(CXX) $(CFLAGS) $(INCLUDES) -c -o $@ $<
