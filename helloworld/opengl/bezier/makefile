CC=gcc
CFLAGS=-g -Wall -Wfatal-errors

all: test

test: main.c
	$(CC) $< $(CFLAGS) -o $@ -lglut -lGL -lGLU

clean:
	rm -f *.o test