#Einfaches Makefile, ohne clean, etc.
#Compiler, wieso nicht standard?
#CC=gcc
#Verzeichnise
Include=.
#Warnings & Co
Cflags=-g -W -Wall
#Fuer den Linker
#CLfmysql=-L/usr/lib/mysql -lmysqlclient
#Compilepath
Comppath=~/exec/

ALL: solreader

solreader: solreader.c
	$(CC) $< -o $(Comppath)$@ -I$(Include) $(Cflags) $(CLflags)
