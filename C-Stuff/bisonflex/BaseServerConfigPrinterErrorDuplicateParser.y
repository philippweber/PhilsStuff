%{

#include <stdio.h>
#include <stdlib.h>

extern int yylineno;
extern char*yytext;

struct serverstruct{
	unsigned int packetsize;
	unsigned char*name;
	unsigned int port;
	unsigned char*directory;
	unsigned char*prefix;
	unsigned char*suffix;
};
struct serverstructlist{
	struct serverstructlist*next;
	struct serverstruct item;
};

struct serverstructlist*top;
struct serverstructlist*last;

void printss(struct serverstructlist*ssl);/*Weiter unten :)*/
void yyerror(char*s);/*Weiter unten :)*/

%}

%union{
	unsigned char*Charstring;
	unsigned int Zahl;
	struct serverstructlist*serverstructlistpointer;
}

%token <serverstructlistpointer> servertoken endtoken packetsizetoken nametoken
%token porttoken directorytoken prefixtoken suffixtoken <Zahl> numbertoken
%token <Charstring> anstringtoken /*Alphanummerischer String*/ <Charstring> dirstringtoken /*In "" fuer Verzeichnisse*/
%token linetoken /*Zeilenumbruch*/
%start Start

%type <serverstructlistpointer> Option
%type <serverstructlistpointer> Configuration
%type <serverstructlistpointer> Server
%type <serverstructlistpointer> Start

%%

Start: Server {last=top=$1;}
	|	Start Server {/*Struktur ablegen*/
									if(last==NULL)last=$2;
									if(top==NULL)top=$2;
									if($2!=NULL){
										last->next=$2;
										last=$2;
									}
									$$=$1;
									/*$<serverstructlistpointer>$ anstelle von $$ auch moeglich*/
								}
	|	Start LinesSpaces
	|	LinesSpaces
	|	error {yyerror("'Keine Serverstruktur vorhanden'");} linetoken {yyclearin;yyerrok;$$=NULL;}
	;

Server:	servertoken {/*Struktur anlegen*/
											$$=(struct serverstructlist*)calloc(1,sizeof(struct serverstructlist));
										} linetoken {
											$$=$<serverstructlistpointer>2
										} Configuration endtoken servertoken linetoken {/*Struktur pruefen*/
											printss($<serverstructlistpointer>2);
											$$=$<serverstructlistpointer>2;
										}
	|	error {yyerror("'Probleme in der Serverstruktur'");} linetoken {yyclearin;yyerrok;$$=NULL;}
	;

Configuration: Option {$$==$<serverstructlistpointer>1;}
	|	Option Configuration {$$=$<serverstructlistpointer>1;}
	;

Option: LinesSpaces {$$=$<serverstructlistpointer>0;}
	|	packetsizetoken numbertoken	{/*Zahl initialisieren*/
																	if($<serverstructlistpointer>0==NULL)yyerror("Serverstruct noch nicht initialisiert");
																	if($<serverstructlistpointer>0->item.packetsize!=0)yyerror("'Wert doppelt vorhanden: Packetsize'");
																	$<serverstructlistpointer>0->item.packetsize=$2;
																} linetoken {$$=$<serverstructlistpointer>0;}
	|	nametoken anstringtoken	{/*Namensstring initialisieren*/
															if($<serverstructlistpointer>0==NULL)yyerror("Serverstruct noch nicht initialisiert");
															if($<serverstructlistpointer>0->item.name!=0)yyerror("'Wert doppelt vorhanden: Name'");
															$<serverstructlistpointer>0->item.name=$2;
														} linetoken {$$=$<serverstructlistpointer>0;}
	|	porttoken numbertoken	{/*Zahl initialisieren*/
														if($<serverstructlistpointer>0==NULL)yyerror("Serverstruct noch nicht initialisiert");
														if($<serverstructlistpointer>0->item.port!=0)yyerror("'Wert doppelt vorhanden: Port'");
														$<serverstructlistpointer>0->item.port=$2;
													} linetoken {$$=$<serverstructlistpointer>0;}
	|	directorytoken dirstringtoken	{/*Verzeichnis initialisieren*/
																		if($<serverstructlistpointer>0==NULL)yyerror("Serverstruct noch nicht initialisiert");
																		if($<serverstructlistpointer>0->item.directory!=0)yyerror("'Wert doppelt vorhanden: Directory'");
																		$<serverstructlistpointer>0->item.directory=$2;
																	} linetoken {$$=$<serverstructlistpointer>0;}
	|	prefixtoken dirstringtoken	{/*Praefix initialisieren*/
																	if($<serverstructlistpointer>0==NULL)yyerror("Serverstruct noch nicht initialisiert");
																	if($<serverstructlistpointer>0->item.prefix!=0)yyerror("'Wert doppelt vorhanden: Prefix'");
																	$<serverstructlistpointer>0->item.prefix=$2;
																} linetoken {$$=$<serverstructlistpointer>0;}
	|	suffixtoken dirstringtoken	{/*Suffix initialisieren*/
																	if($<serverstructlistpointer>0==NULL)yyerror("Serverstruct noch nicht initialisiert");
																	if($<serverstructlistpointer>0->item.suffix!=0)yyerror("'Wert doppelt vorhanden: Suffix'");
																	$<serverstructlistpointer>0->item.suffix=$2;
																} linetoken {$$=$<serverstructlistpointer>0;}
	|	error {yyerror("'Option nicht erkannt'");} linetoken {yyclearin;$$=$<serverstructlistpointer>0;yyerrok;}
	;

LinesSpaces: linetoken | LinesSpaces linetoken;

%%

void yyerror(char*s) {
	fprintf(stderr,"Zeile %d: %s mit '%s' \n",yylineno,s,yytext);
}

void printss(struct serverstructlist*ssl){
		printf("Server:\n");
		if(ssl->item.packetsize!=0)printf("Packetsize: %u\n",ssl->item.packetsize);
		if(ssl->item.name!=NULL)printf("Name: %s\n",ssl->item.name);
		if(ssl->item.port!=0)printf("Port: %u\n",ssl->item.port);
		if(ssl->item.directory!=NULL)printf("Directory: %s\n",ssl->item.directory);
		if(ssl->item.prefix!=NULL)printf("Prefix: %s\n",ssl->item.prefix);
		if(ssl->item.suffix!=NULL)printf("Suffix: %s\n",ssl->item.suffix);
		printf("End Server\n\n");
}

int main(void) {
	yyparse();
	printf("\n\n\nParse zu Ende\n\n\n");
	struct serverstructlist*nextss,*prevss;
	prevss=NULL;
	for(nextss=top;nextss!=NULL;nextss=nextss->next){
		if(prevss==nextss){prevss=NULL;break;}//Endlosschleife, falls nur ein Strukt vorhanden...
		free(prevss);
		prevss=nextss;
		printf("Server:\n");
		if(nextss->item.packetsize!=0)printf("Packetsize: %u\n",nextss->item.packetsize);
		if(nextss->item.name!=NULL)printf("Name: %s\n",nextss->item.name);
		if(nextss->item.port!=0)printf("Port: %u\n",nextss->item.port);
		if(nextss->item.directory!=NULL)printf("Directory: %s\n",nextss->item.directory);
		if(nextss->item.prefix!=NULL)printf("Prefix: %s\n",nextss->item.prefix);
		if(nextss->item.suffix!=NULL)printf("Suffix: %s\n",nextss->item.suffix);
		printf("End Server\n\n");
	}
	free(prevss);
	return(0);
}
