#Einfaches Makefile, ohne clean, etc.
#Compiler, wieso nicht standard?
CC=gcc
#Verzeichnise
Include=.
#Warnings & Co
Cflags=-g -W -Wall
#Fuer den Linker
CLfmysql=-L/usr/lib/mysql -lmysqlclient
#Compilepath
Comppath=~/exec/

ALL: testaquery

testaquery: testaquery.c
	$(CC) $< -o $(Comppath)$@ -I$(Include) $(Cflags) $(CLfmysql)
