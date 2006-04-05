CC = gcc
DEBUG = -Wall -g 
INCLUDES = -I. -I/usr/local/lib
LIBS = -L/usr/local -lxar -lcrypto -lsqlite3
COMPILE_FLAGS = -c
INSTALL = /usr/bin/install
PREFIX = /usr/local

all: xidb.o xiarchive.o
	$(CC) $(INCLUDES) $(LIBS) $(DEBUG) -o xi main.c \
			xidb.o xiarchive.o

xidb.o:
	$(CC) $(COMPILE_FLAGS) $(INCLUDES) $(LIBS) $(DEBUG) xidb.c 

xiarchive.o:
	$(CC) $(COMPILE_FLAGS) $(INCLUDES) $(LIBS) $(DEBUG) xiarchive.c

clean:
	rm *.o xi 

install:
	$(INSTALL) -b xi $(PREFIX)/bin
	$(INSTALL) xi.1 $(PREFIX)/man/man1
	/usr/bin/touch /Users/olegb/xi.db
	chmod 775 /Users/olegb/xi.db

uninstall:
	rm $(PREFIX)/bin/xi
