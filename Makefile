# Variables
CC = gcc
CFLAGS = $(shell sdl2-config --cflags)
LDFLAGS = $(shell sdl2-config --libs)
SRC = src/main.c
OUT = main

# Règles
all: $(OUT)

$(OUT): $(SRC)
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)

clean:
	rm -f $(OUT)

.PHONY: all clean