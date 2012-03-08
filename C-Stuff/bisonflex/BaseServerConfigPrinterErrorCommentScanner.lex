/*Konfigurationsscanner*/

/*strncpy of yytext mit yyleng*/

%{
#include <stdio.h>
#include <string.h>
#include <BaseServerConfigPrinterErrorDuplicateParser.tab.h>
%}

DIGIT [0-9]
ALPHA [a-zA-Z]
DIRCHAR [/._~]

%option yylineno

%%

server return(servertoken);
end return(endtoken);
packetsize return(packetsizetoken);
name return(nametoken);
port return(porttoken);
directory return(directorytoken);
prefix return(prefixtoken);
suffix return(suffixtoken);

{DIGIT}+ {yylval.Zahl=(unsigned int)atoi(yytext);return(numbertoken);}
({ALPHA}|{DIGIT})+ {yylval.Charstring=(unsigned char*)strdup(yytext);return(anstringtoken);}
\"({ALPHA}|{DIGIT}|{DIRCHAR})+\" {yylval.Charstring=(unsigned char*)strdup(yytext);return(dirstringtoken);}

[\n]+ return(linetoken);//Linebreaks
[ \t]+ ;//Gegen die Leerzeichen und Tabulator

\/\*([^(\*\/)])*\*\/ ; //Kommentar in /* */

\/\/.* ; //Kommentar mit //

%%
