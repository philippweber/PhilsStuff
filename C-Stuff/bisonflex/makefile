#Compiler
CC=gcc
LEX=flex
YACC=bison
#Verzeichnise
Include=.
#Warnings & Co
Cflags=-g -W -Wall
#Fuer den Linker
CLflags=-lfl
#Compilepath
Comppath=~/exec/

ALL: BaseServerConfigTokenizer BaseServerConfigPrinter BaseServerConfigPrinterError BaseServerConfigPrinterErrorDuplicate

BaseServerConfigTokenizer: BaseServerConfigTokenizer.c
	$(CC) $< -o $(Comppath)$@ -I$(Include) $(Cflags)

BaseServerConfigTokenizer.c: BaseServerConfigTokenizer.lex
	$(LEX) -o $@ $<

BaseServerConfigPrinter: BaseServerConfigPrinterParser.o BaseServerConfigPrinterScanner.o 
	$(CC) $^ -o $(Comppath)$@ -I$(Include) $(Cflags) $(CLflags)

BaseServerConfigPrinterParser.o: BaseServerConfigPrinterParser.tab.c
	$(CC) -c $< -o $@ -I$(Include) $(Cflags)

BaseServerConfigPrinterParser.tab.c: BaseServerConfigPrinterParser.y
	$(YACC) -d $<

BaseServerConfigPrinterScanner.o: BaseServerConfigPrinterScanner.c #BaseServerConfigPrinterParser.h
	$(CC) -c $< -o $@ -I$(Include) $(Cflags)

BaseServerConfigPrinterScanner.c: BaseServerConfigPrinterScanner.lex
	$(LEX) -o $@ $<

BaseServerConfigPrinterError: BaseServerConfigPrinterErrorParser.o BaseServerConfigPrinterErrorScanner.o 
	$(CC) $^ -o $(Comppath)$@ -I$(Include) $(Cflags) $(CLflags)

BaseServerConfigPrinterErrorParser.o: BaseServerConfigPrinterErrorParser.tab.c
	$(CC) -c $< -o $@ -I$(Include) $(Cflags)

BaseServerConfigPrinterErrorParser.tab.c: BaseServerConfigPrinterErrorParser.y
	$(YACC) -d $<

BaseServerConfigPrinterErrorScanner.o: BaseServerConfigPrinterErrorScanner.c #BaseServerConfigPrinterErrorParser.h
	$(CC) -c $< -o $@ -I$(Include) $(Cflags)

BaseServerConfigPrinterErrorScanner.c: BaseServerConfigPrinterErrorScanner.lex
	$(LEX) -o $@ $<

BaseServerConfigPrinterErrorDuplicate: BaseServerConfigPrinterErrorDuplicateParser.o BaseServerConfigPrinterErrorCommentScanner.o 
	$(CC) $^ -o $(Comppath)$@ -I$(Include) $(Cflags) $(CLflags)

BaseServerConfigPrinterErrorDuplicateParser.o: BaseServerConfigPrinterErrorDuplicateParser.tab.c
	$(CC) -c $< -o $@ -I$(Include) $(Cflags)

BaseServerConfigPrinterErrorDuplicateParser.tab.c: BaseServerConfigPrinterErrorDuplicateParser.y
	$(YACC) -d $<

BaseServerConfigPrinterErrorCommentScanner.o: BaseServerConfigPrinterErrorCommentScanner.c #BaseServerConfigPrinterErrorDuplicateParser.h
	$(CC) -c $< -o $@ -I$(Include) $(Cflags)

BaseServerConfigPrinterErrorCommentScanner.c: BaseServerConfigPrinterErrorCommentScanner.lex
	$(LEX) -o $@ $<
	