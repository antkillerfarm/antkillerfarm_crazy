D			[0-9]

%{
    #include "y.tab.h"
    #include <stdlib.h>
%}

%%
"SetBaud"		{ printf("L:SETBAUD\r\n"); return SETBAUD; }
"SetPort"		{ printf("L:SETPORT\r\n"); return SETPORT; }
"SetBlock"		{ return SETBLOCK; }
"Send"			{ printf("L:SEND\r\n"); return SEND; }

{D}+			{ 
			   printf("L:NUMBER\r\n"); 
			   yylval.int_value = atoi(yytext); 
			   return NUMBER;
			}

\".*\"			{ 
			   int len = strlen(yytext);
			   yylval.str = strndup(yytext + 1, len - 2);
			   printf("L:CMD:%d:%s\r\n", len, yylval.str);  
			   return CMD;
			}

[;]      		{ return *yytext; }

[ \t\n]   ;       /* skip whitespace */

%%

int yywrap(void) {
    return 1;
}
