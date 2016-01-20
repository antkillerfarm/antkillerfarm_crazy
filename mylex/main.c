#include <stdio.h>
#include <stdlib.h>
#include "mylex.h"

void SetBaud(int _baud)
{
	printf("Baud:%d\r\n", _baud);
}

void SetPort(int _port)
{
	printf("Port:%d\r\n", _port);
}

void SendCmd(char* _s)
{
	printf("Cmd:%s\r\n", _s);
	free(_s);
}

int main(void) {
	FILE* f = fopen("comand.mac", "rb");
	yyset_in(f);
	yyparse();
	//yylex();
	//yylex();
	fclose(f);
	return 0;
}
