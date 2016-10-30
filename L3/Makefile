###                             MAKEFILE                            ####
### Makefile for the threads                                        ####
###                                                                 ####
###    -lpthreads is for including POSIX Threads                    ####
###    -lmultiply_matrix is for including matrix multiplication     ####
###    -L._use libraries in the current directory the other flags   ####
###        are predefinded                                          ####

CC=clang
CFLAGS=-Wall -pedantic -Werror -O3
LDFLAGS=-lpthread
AR=ar
ARFLAGS=rcs
TARGETS=mthread work multiply_matrix matrix

all: $(TARGETS)

# Compile as library
multiply_matrix:
	$(CC) $(CFLAGS) -c $@.c -o $@.o
	$(AR) $(ARFLAGS) libmultiplymatrix.a $@.o

matrix: multiply_matrix
	$(CC) $(CFLAGS) $@.c $(LDFLAGS) -L. -lmultiplymatrix -o $@

# Just to use auto-completion in shell
work:

mthread: gitversion.c

gitversion.c: ../.git/HEAD ../.git/index
	echo "const char *gitversion = \"$(shell git rev-parse HEAD)\";" > $@

%: %.c
	$(CC) $(CFLAGS) $(LDFLAGS) $< -o $@

# CleanUp .......................................#

clean:
	rm -f *.o $(TARGETS) gitversion.c libmultiplymatrix.a
