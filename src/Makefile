.SILENT:

COPTS = -Wall -ansi -pedantic -c -O3
STRIP = strip
COPTSDEBUG = -g
LINT = splint
LINTOPTS = -strict-lib
VALGRIND = valgrind
VALGRINDOPTS = -q --leak-check=yes
CC = gcc
LD = $(CC)
LDOPTS = -o

lint:
	find ./ -follow | grep -E '([a-zA-Z0-9 _-.])+\.(c|h)' | xargs $(LINT) $(LINTOPTS)

.c.o:
	echo Compiling $<
	$(CC) $(COPTS) $<

clean:
	echo Cleaning
	- rm -r *.o
