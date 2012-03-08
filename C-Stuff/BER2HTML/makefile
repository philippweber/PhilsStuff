#Einfaches Makefile, ohne clean, etc.
#Compiler, wieso nicht standard?
CC=gcc
#Verzeichnise
Include=.
#Warnings & Co
Cflags=-g -W -Wall
#Fuer den Linker
CLflags=
#Compilepath
Comppath=~/exec/

ALL: BER2HTML BER2HTMLPro

BER2HTML: BER2HTML.c
	$(CC) $< -o $(Comppath)$@ -I$(Include) $(Cflags) $(CLflags)
BER2HTMLPro: BER2HTMLPro.c
	$(CC) $< -o $(Comppath)$@ -I$(Include) $(Cflags) $(CLflags)
