all: matrixgen.exe

matrixgen.exe: main.o
	gcc -o $@ $^

main.o: main.c
	gcc -g -o $@ -c main.c

clean:
	rm *.exe *.o


