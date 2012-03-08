#ifndef tokens
	//Gegen Compilerbeschwerden
	extern FILE *yyin;
	extern int yylex (void);
	extern char *yytext;
	//Ende der Compilerbeschwerden
	#define tokens
	#define servertoken 1
	#define porttoken 2
	#define usertoken 3
	#define passwordtoken 4
	#define dbnametoken 5
	#define numbertoken 6
	#define stringtoken 7
	#define passwordstringtoken 8
#endif
