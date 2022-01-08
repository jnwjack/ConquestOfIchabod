SHELL = /bin/bash

CFLAG = -Wall -g
CC = gcc
ENTRY = main.c
OUTPUT = coi
LIBS = -lSDL2 -lSDL2main

build:
	$(CC) $(ENTRY) $(CFLAGS) $(LIBS) -o $(OUTPUT)

clean:
	rm -f *.o $(OUTPUT) *~

