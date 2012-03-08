#Einfaches Makefile, ohne clean, etc.
#Compiler, wieso nicht standard?
CC=gcc
LEX=flex
#Verzeichnise
Include=.
#Warnings & Co
CFlags=-g -W -Wall
#Fuer den Linker
CLFcrypto=-lssl -lcrypto
CLFmysql=-L/usr/lib/mysql -lmysqlclient
CLFflex=-lfl
#Compilepath
Comppath=~/exec/

ALL: mysqlkeys

mysqlkeys: mysqlkeys.c mysqlconfscan.c
	$(CC) $^ -o $(Comppath)$@ -I$(Include) $(CFlags) $(CLFcrypto) $(CLFmysql) $(CLFflex)

mysqlconfscan.c: mysqlconfscan.lex
	$(LEX) -o $@ $<
