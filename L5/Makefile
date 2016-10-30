###                             MAKEFILE                            ####
###           Makefile for the scheduling parameter query           ####
###                                                                 ####
###    -lpthreads       is for including POSIX Threads library      ####
###    -Wall -pedantic  print errors on langugage extensions        ####

CC=clang
CFLAGS=-Wall -pedantic -g
LDFLAGS=-lpthread
OBJECTS=gitversion.o schedcheck.o
TARGETS=schedcheck

all: $(TARGETS)

schedcheck: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) $(LDFLAGS) -o $@

gitversion.c: ../.git/HEAD ../.git/index
	echo "const char *gitversion = \"$(shell git rev-parse HEAD)\";" > $@

%.o: %.c
	$(CC) -c $(CFLAGS) $<

%: %.c
	$(CC) $(CFLAGS) $(LDFLAGS) $< -o $@


# CleanUp                                                              #
.PHONY: clean
clean:
	rm -f $(OBJECTS) $(TARGETS) gitversion.c
