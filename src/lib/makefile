.SILENT:
.PHONY : clean
TARGET = game
TARGET_DEPS = screen.o dungeonCrawlerFront.o dungeonCrawlerBack.o libparse.o libcrawl.o strings.o state.o
COPTS = -Wall -ansi -pedantic -O3 -c
STRIP = strip
COPTSDEBUG = -g
LINT = splint
LINTOPTS = -strict-lib
VALGRIND = valgrind
VALGRINDOPTS = -q --leak-check=yes --track-origins=yes -v --log-file=valgrind.log --show-reachable=yes --malloc-fill=CA --free-fill=FE
CC = gcc
LD = $(CC)
LDOPTS = -o

SCREEN = screen.c screen.h
LIBPARSE = libparse.c libparse.h
STRINGS = strings.c strings.h
LIBCRAWL = libcrawl.c libcrawl.h
BACK = dungeonCrawlerBack.c dungeonCrawlerBack.h


$(TARGET):	$(TARGET_DEPS)
	$(LD) $(LDOPTS) $(TARGET) $(TARGET_DEPS)
	$(STRIP) $(TARGET)

screen.o:	$(SCREEN)
dungeonCrawlerFront.o:	$(SCREEN) $(BACK) $(LIBCRAWL) $(STATE)
libparse.o:	$(LIBPARSE) $(STRINGS)
libcrawl.o:	$(LIBPARSE) $(STRINGS) $(LIBCRAWL)
dungeonCrawlerBack.o:	$(BACK) $(LIBCRAWL) $(STATE)
state.o:	$(LIBCRAWL) $(LIBPARSE)
strings.o:	$(STRINGS)

.c.o:
	echo Compiling $<
	$(CC) $(COPTS) $<

lint:
	find ./ -follow | grep -E '([a-zA-Z0-9 _-.])+\.(c|h)$$' | xargs $(LINT) $(LINTOPTS)


clean:
	echo Cleaning
	- rm -r *.o
	- rm game
