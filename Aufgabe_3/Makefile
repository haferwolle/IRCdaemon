CC=g++
PROG=a3
LDFLAGS=-ldl

all: $(PROG)

a3: main.cpp logconsole.so logfile.so calc.so
		$(CC) -rdynamic main.cpp -o $(PROG) $(LDFLAGS) -L. -lcalc
		
calc.so: calcAreas.c calcNewton.c
		$(CC) -c calcAreas.c -o calcAreas.o
		$(CC) -c calcNewton.c -o calcNewton.o
		$(CC) -shared -o libcalc.so calcAreas.o calcNewton.o
		
logconsole.so: logconsole.c
		$(CC) -c logconsole.c -o logconsole.o
		$(CC) -shared -o liblogconsole.so logconsole.o
		
logfile.so: logfile.c
		$(CC) -c logfile.c -o logfile.o
		$(CC) -shared -o liblogfile.so logfile.o
		
clean:
		rm -rf *.o *.so $(PROG) log.txt
		