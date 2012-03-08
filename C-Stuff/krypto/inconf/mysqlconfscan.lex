%{
#include "mysqltokens.h" 
%}

digit [0-9]
letter [A-Za-z0-9_]
/*password_letter [A-Za-z0-9_$;.:,!?"'/\\]*/
useless [ \t\n]+

%%

Server {printf("Server: %s\n",yytext);return servertoken;}
Port {printf("Port: %s\n",yytext);return porttoken;}
User {printf("User: %s\n",yytext);return usertoken;}
Password {printf("Password: %s\n",yytext);return passwordtoken;}
Database {printf("Database: %s\n",yytext);return dbnametoken;}

{digit}* {printf("Number: %s\n",yytext);return numbertoken;}

\"{letter}*\" {printf("String: %s\n",yytext);return stringtoken;}

\"[^\"]*\" {printf("PasswordString: %s\n",yytext);return passwordstringtoken;}/*Alles fuers Passwort*/

{useless} ;

\/\*([^(\*\/)])*\*\/ ; //Kommentar in /* */

\/\/.* ; //Kommentar mit //

%%
  