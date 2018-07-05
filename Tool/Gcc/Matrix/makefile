all: matrix.exe

matrix.exe: matrix.o main.o
	gcc -o $@ $^

matrix.o: matrix.c types.h
	gcc -g -o $@ -c matrix.c

main.o: main.c
	gcc -g -o $@ -c main.c

clean:
	rm *.exe *.o


