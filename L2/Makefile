#-------------------------------------------------------------------------------#
# -Wall -Wpedantic -Werror - Mehr Fehlermeldungen                               #
# AR                       - Archiver zum Erstellen von statischen Bibliotheken #
# ARFLAGS                  - Argumente fuer static library                      #
# TESTS                    - Quelldateien zu allen Tests                        #
# SOURCES                  - Alle C-Quelldateien in L2                          #
# HEADERS                  - Alle Headerdateien in L2                           #
# OBJECTS                  - Die Objects zu den Quellen                         #
#-------------------------------------------------------------------------------#

CC=clang
CFLAGS=-Wall -Wpedantic -Werror -g
LDFLAGS=-L. -lgenstacklib
AR=ar
ARFLAGS=rcs
TESTS=gentest1 gentest2 gentest3 gentest4
OBJECTS=genstacklib.o gentest1.o gentest2.o

all: genstacklib gentests

genstacklib:
	$(CC) $(CFLAGS) -c $@.c -o $@.o
	$(AR) $(ARFLAGS) lib$@.a $@.o

gentests: $(TESTS)

gentest1: gentest1.c genstacklib
	$(CC) -o $@ $< $(LDFLAGS)

gentest2: gentest2.c genstacklib
	$(CC) -o $@ $< $(LDFLAGS)

gentest3: gentest3.c genstacklib
	$(CC) -o $@ $< $(LDFLAGS)

gentest4: gentest4.c genstacklib
	$(CC) -o $@ $< $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) $< -o $@

.PHONY: clean
clean:
	rm -f $(OBJECTS) libgenstacklib.a $(TESTS)
