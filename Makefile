CC=gcc
CFLAGS=-O3 -Wall -Wextra -Werror

all: convert

convert: convert.o
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $^

.PHONY: clean

clean:
	rm -rf convert *.o *.pch
