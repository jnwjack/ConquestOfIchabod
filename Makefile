SHELL = /bin/bash

CFLAG = -Wall -g
CC = gcc
ENTRY = src/main.c
SRCS = $(shell find src -name "*.c")
OUTPUT = coi
LIBS = -lSDL2 -lSDL2main -lSDL2_image -lSDL2_ttf

build:
	$(CC) $(SRCS) $(CFLAGS) $(LIBS) -o $(OUTPUT)

clean:
	rm -f *.o $(OUTPUT) *~ src/*~ src/engine/*~
