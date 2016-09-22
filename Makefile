CC=gcc
CFLAGS=-Wall -fPIC -I./include -std=c99
LDFLAGS=-lm
SOURCES=$(wildcard src/*.c)
OBJECTS=$(patsubst %.c,%.o,$(SOURCES))

all: helios

helios: $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

%.o: src/%.c include/%.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf src/*.o helios
