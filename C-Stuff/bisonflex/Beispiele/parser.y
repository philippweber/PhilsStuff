/*%code top {
	#define PARSER
}*/

%{

#include <stdio.h>

%}

%token LeftBrace LeftParen String RightParen RightBrace

%start Input

%%

Input:
    /*Empty*/
  | Input Brace  {printf("Anfang/Ende -> Braces {}\n");}
  ;

Expression:  Expression Expression  {printf("Expression -> Expression + Expression\n");}
          |  Brace  {printf("Expression -> Braces {}\n");}
          |  Paren  {printf("Expression -> Paren ()\n");}
          |  String  {printf("Expression -> String\n");};
          ;

Brace: LeftBrace Expression RightBrace  {printf("Braces {} -> Expression\n");};

Paren: LeftParen Expression RightParen  {printf("Paren () -> Expression\n");};

//Hier noch ein paar andere Moeglichkeiten...

/*Expression:  Expression Expression
          |  LeftBrace Brace RightBrace
          |  LeftParen Paren RightParen
          |  String
          ;

Brace: Expression;

Paren: Expression;*/

/*Brace:  Brace Brace  {printf("Braces + Braces {}{}\n");}
     |  LeftBrace Brace RightBrace  {printf("Braces with Braces {}\n");}
     |  LeftBrace Paren RightBrace  {printf("Braces with Paren {}\n");}
     |  LeftBrace String RightBrace  {printf("Braces with String {}\n");}
     ;

Paren:  Paren Paren  {printf("Paren + Paren ()()\n");}
     |  LeftParen Brace RightParen  {printf("Paren with Braces ()\n");}
     |  LeftParen Paren RightParen  {printf("Paren with Paren ()\n");}
     |  LeftParen String RightParen  {printf("Paren with String ()\n");}
     ;*/

%%

void yyerror(char *s) {
  printf("%s\n",s);
}

int main(void) {
  yyparse();
  return 0;
}
