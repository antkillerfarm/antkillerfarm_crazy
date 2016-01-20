%{
#include <stdio.h>

void yyerror(char *s);
void SetBaud(int _baud);
void SetPort(int _port);
void SendCmd(char* _s);
%}

%union{
    int int_value;
    char *str;
}

%token SETBAUD SETPORT SETBLOCK SEND
%token<int_value> NUMBER
%token<str> CMD

%%
stmt_list:
          stmt                  { }
        | stmt_list stmt        { }
        ;

stmt:
	SETBAUD	NUMBER ';'	{ SetBaud($2); }
	| SETPORT NUMBER ';'	{ SetPort($2); }
	| SEND CMD ';'		{ SendCmd($2); }
        ;

%%

void yyerror(char *s) {
	fprintf(stdout, "%s\n", s);
}
