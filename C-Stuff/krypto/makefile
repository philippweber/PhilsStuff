#Einfaches Makefile, ohne clean, etc.
#Compiler, wieso nicht standard?
CC=gcc
#Verzeichnise
Include=.
#Warnings & Co
Cflags=-g -W -Wall
#Fuer den Linker
CLflags=-lssl -lcrypto
CLfmysql=-L/usr/lib/mysql -lmysqlclient
#Compilepath
Comppath=~/exec/

ALL: destest md5test mysqlkeys mysqlexample

destest: destest.c
	$(CC) $< -o $(Comppath)$@ -I$(Include) $(Cflags) $(CLflags)

md5test: md5test.c
	$(CC) $< -o $(Comppath)$@ -I$(Include) $(Cflags) $(CLflags)

mysqlkeys: mysqlkeys.c
	$(CC) $< -o $(Comppath)$@ -I$(Include) $(Cflags) $(CLflags) $(CLfmysql)

mysqlexample: mysqlexample.c
	$(CC) $< -o $(Comppath)$@ -I$(Include) $(Cflags) $(CLflags) $(CLfmysql)
	