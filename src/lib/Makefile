.SILENT:

TARGET = game
TARGET_DEPS = dungeonCrawlerFront.o dungeonCrawlerBack.o libparse.o libcrawl.o strings.o state.o getnum.o
COPTS = -Wall -ansi -pedantic
STRIP = strip
COPTSDEBUG = -g
LINT = splint
LINTOPTS = -strict-lib
VALGRIND = valgrind
VALGRINDOPTS = -q --leak-check=yes --track-origins=yes -v --log-file=valgrind.log --show-reachable=yes --malloc-fill=CA --free-fill=FE
CC = gcc
LD = $(CC)
LDOPTS = -o

$(TARGET):
	$(CC) $(COPTS) -o $(TARGET) dungeonCrawlerBack.c dungeonCrawlerFront.c getnum.c libcrawl.c libparse.c state.c strings.c screen.c -g

lint:
	find ./ -follow | grep -E '([a-zA-Z0-9 _-.])+\.(c|h)' | xargs $(LINT) $(LINTOPTS)

clean:
	echo Cleaning
	- rm -r *.o
