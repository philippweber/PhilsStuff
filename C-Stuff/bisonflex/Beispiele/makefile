#Compiler
CC=gcc
LEX=flex
YACC=bison
#Verzeichnise
Include=.
#Warnings & Co
Cflags=-g -W -Wall
#F�r den Linker
CLflags=-lfl
#Compilepath
Comppath=~/exec/


ALL: Parser Scanner SimpleScanner SimpleParser

Parser: parser.tab.c scanner.c
	$(CC) $^ -o $(Comppath)$@ -I$(Include) $(Cflags) $(CLflags) -D=PARSER

parser.tab.c: parser.y
		$(YACC) -d $<

Scanner: scanner.c
	$(CC) $< -o $(Comppath)$@ -I$(Include) $(Cflags) $(CLflags)

scanner.c: scanner.lex
	$(LEX) -o $@ $<

SimpleScanner: simples.c
	$(CC) $< -o $(Comppath)$@ -I$(Include) $(Cflags) $(CLflags)

simples.c: simples.lex
	$(LEX) -o $@ $<

SimpleParser: simplep.tab.c
	$(CC) $< -o $(Comppath)$@ -I$(Include) $(Cflags)

simplep.tab.c: simplep.y
		$(YACC) -d $<
