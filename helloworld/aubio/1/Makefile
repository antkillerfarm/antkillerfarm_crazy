CC       = gcc
FLAGS    = -Wall -g
INCLUDES = `pkg-config --cflags --libs aubio`

all: hello

hello: hello.c
	$(CC) -o hello hello.c $(INCLUDES) $(FLAGS)

clean: hello
	rm hello
