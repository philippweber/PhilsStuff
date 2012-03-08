%{

#include <stdio.h>

#ifndef PARSER
	#define LeftBraceAction printf("LeftBrace\n");
	#define LeftParenAction printf("LeftParen\n");
	#define RightParenAction printf("RightParen\n");
	#define RightBraceAction printf("RightBrace\n");
	#define StringAction {printf("String: %s\n",yytext);}
#else
	#include <parser.tab.h>
	#define LeftBraceAction return(LeftBrace);
	#define LeftParenAction return(LeftParen);
	#define RightParenAction return(RightParen);
	#define RightBraceAction return(RightBrace);
	#define StringAction {printf("String: %s\n",yytext);return(String);}
#endif

%}

letter [A-Za-z0-9]

useless [ \t\n]+

%%

"{"  LeftBraceAction
"("  LeftParenAction
")"  RightParenAction
"}"  RightBraceAction

{letter}*  StringAction

{useless}

%%

/*#ifndef PARSER
int main(int argc, char **argv)
{
  if(argc>1)
    yyin=fopen(argv[1],"r");
  else return 0;
  yylex();
  printf("\n");
  return 0;
}
#endif*/

