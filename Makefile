SHELL = /bin/bash

CFLAG = -Wall -g
CC = gcc
ENTRY = src/main.c
SRCS = $(shell find src -name "*.c")
OUTPUT = coi
LIBS = -lSDL2 -lSDL2main -lSDL2_image -lSDL2_ttf

# TOOLS
SRCS_TOOLS = tools/cbb.c src/engine/util.c
CFLAG_TOOLS = -g

build:
	$(CC) $(SRCS) $(CFLAGS) $(LIBS) -o $(OUTPUT)

clean:
	rm -f *.o $(OUTPUT) *~ src/*~ src/engine/*~ src/.#* src/engine/.#*

tool:
	$(CC) $(SRCS_TOOLS) `pkg-config gtk4 --cflags pkg-config gtk4 --libs` -o cbb 
