CC       = gcc
FLAGS    = -Wall -O1 -g
INCLUDES = `pkg-config --cflags --libs webkitgtk-3.0`

all: minibrowser

minibrowser: minibrowser.c
	$(CC) -o minibrowser minibrowser.c $(INCLUDES) $(FLAGS)

clean: minibrowser
	rm minibrowser
	rm *~

