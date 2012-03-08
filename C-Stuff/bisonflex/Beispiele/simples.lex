%{
#include <stdio.h>//Falls noetig gehen auch weitere Header
#define STARTCAESAR 3//Definitionen hierhin...
char CAESAR=STARTCAESAR;//"Konstante" als Verschiebechiffre
%}

	/* Deklarationen */
GrossBuchstabe [A-Z]
KleinBuchstabe [a-z]

%%

	/* Vorher deklarierte Einheiten */ 
{GrossBuchstabe} dechiffre(CAESAR);
{KleinBuchstabe} enchiffre(CAESAR);
	/* Geht auch direkt */
[0-9] {CAESAR=yytext[0]-'0';printf("\nNeue Verschiebechiffre: %hu\n",CAESAR);}
	/* Tabulatoren */
[\t]+ printf("\nVerschiebechiffre: %hu\n",CAESAR);
	/* Spezialfaelle */
42 printf("\nLife, Universe & Everything\n");
CAESAR|PHILIPP printf("\nDer Groesste\n");

%%

	/* Funktionen, hier koennte auch eine Main hin */

void dechiffre(char start){
	printf("%c",((yytext[0]-'A'-start+26)%26)+'A');
}

void enchiffre(char start){
	printf("%c",((yytext[0]-'a'+start)%26)+'a');
}
