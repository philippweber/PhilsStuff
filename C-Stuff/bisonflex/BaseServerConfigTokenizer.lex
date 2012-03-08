/*Konfigurationsscanner*/

%{
  //#include <y.tab.h>
%}

DIGIT [0-9]
ALPHA [a-zA-Z]
/*STRING \"[^\n"]+\"*/

%%

{DIGIT}+ printf("\nEin Integer: %s (%d)\n",yytext,atoi(yytext));

server|end|packetsize|name|port|directory|prefix|suffix printf("KEY(%s) ",yytext);

({ALPHA}|{DIGIT})+ printf("String(%s) ",yytext);

[ \t\n]+ //Gegen die Leerzeichen, Tabulator und Linebreaks...

. printf("Wat is dat? %s\n",yytext);

%%

int yywrap()
{
  return 1;
}

int main(int argc, char **argv)
{
	if(argc>1)yyin=fopen(argv[1],"r");else return(0);
	//if(argc>1)yyrestart(fopen(argv[1],"r"));else return(0);
	yylex();
	printf("\n");
	return(0);
}
