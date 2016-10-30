#----------- Makefile for L1 ---------------#
#-------------------------------------------#
#CFLAGS  vorgegeben, zeigt alle Warnungen   #
#        und Fehler an, erzeugt Debugging-  #
#        Informationen                      #
#LDFLAGS vorgegeben                         #
#PROJECT Name des Projekts, damit der Name  #
#        nicht mehrfach beim erzeugen und   #
#        löschen steht                      #
#-------------------------------------------#

CC=clang
CFLAGS=-Wall -Wpedantic -Werror -g
LDFLAGS=-lm
SOURCES=ulMenuStack.c ulstack.c	# Source files of the project
OBJECTS=ulMenuStack.o ulstack.o	# Object files to generate
PROJECT=ulMenuStack				# Name of the project('s executable)

all: $(PROJECT)

$(PROJECT): $(SOURCES)

%.o: %.c
	$(CC) $(CFLAGS) $(LDFLAGS) $< -o $@

clean:
	rm -f *.o $(PROJECT)
