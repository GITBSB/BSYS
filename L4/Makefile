###                             MAKEFILE                            ####
### Makefile for the threads                                        ####
###                                                                 ####
###    -lpthreads is for including POSIX Threads                    ####
###    -lmultiply_matrix is for including matrix multiplication     ####
###    -L._use libraries in the current directory the other flags   ####
###        are predefinded                                          ####

CC=clang
CFLAGS=-Wall -pedantic -g
LDFLAGS=-lpthread
OBJECTS=gitversion.o producer.o consumer.o
TARGETS=mutex event prodcon

all: $(TARGETS)

prodcon: $(OBJECTS) prodcon.c
	$(CC) $(CFLAGS) prodcon.c $(LDFLAGS) $(OBJECTS) -o $@

# Just to use auto-completion in shell
mutex:

event:

gitversion.c: ../.git/HEAD ../.git/index
	echo "const char *gitversion = \"$(shell git rev-parse HEAD)\";" > $@

%.o: %.c
	$(CC) -c $(CFLAGS) $<

%: %.c
	$(CC) $(CFLAGS) $(LDFLAGS) $< -o $@


# CleanUp .......................................#

clean:
	rm -f $(OBJECTS) $(TARGETS) gitversion.c
