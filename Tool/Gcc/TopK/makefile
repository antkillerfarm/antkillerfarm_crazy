all: topk

topk: topk.o
	gcc -o $@ $^

topk.o: topk.c
	gcc -g -o $@ -c $^

clean:
	rm topk *.o


