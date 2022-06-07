CC=g++
CFLAGS=-g -Wall -Wfatal-errors -std=c++11

all: test

test: test.cpp
	$(CC) $< $(CFLAGS) -o $@

clean:
	rm -f *.o test
