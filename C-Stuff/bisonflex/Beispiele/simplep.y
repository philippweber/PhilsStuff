%{
#include <stdio.h>
%}

%token N O M E V R B

%start Satz

%%

Satz: Subjekt Verb Objekt {printf("Satz gefunden\n");}
	| Subjekt Verb {printf("Satz gefunden\n");};

Subjekt: N {printf("Start Subjekt\t");} O M E N {printf("Subjekt gefunden\n");};

Objekt: N {printf("Start Objekt\t");} O M E N {printf("Objekt gefunden\n");};

Verb: V {printf("Start Verb\t");} E R B {printf("Verb gefunden\n");};

%%

void yyerror(char *s) {
  printf("%s\n",s);
}

int yylex(){
	char c;
	while((c=getchar())!=EOF){
		switch(c){
			case'n':case'N':return N;break;
			case'o':case'O':return O;break;
			case'm':case'M':return M;break;
			case'e':case'E':return E;break;
			case'v':case'V':return V;break;
			case'r':case'R':return R;break;
			case'b':case'B':return B;break;
			case' ':case'\t':break;
			default:return 0;break;
		}
	}
	return 0;
}

int main(void) {
  yyparse();
  return 0;
}
