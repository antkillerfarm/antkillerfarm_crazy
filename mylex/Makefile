all: ly.exe

ly.exe: y.tab.o lex.yy.o main.o
	gcc -o $@ $^

%.o: %.c
	gcc -g -o $@ -c $^

lex.yy.c: mylex.l
	flex $^

y.tab.c: myyacc.y
	bison -y -d myyacc.y

clean:
	rm ly.exe *.o y.tab.c y.tab.h lex.yy.c