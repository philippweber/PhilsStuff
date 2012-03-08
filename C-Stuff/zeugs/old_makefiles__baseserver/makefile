#Einfaches Makefile, ohne clean, etc.
#Moegliche Targets sind: clean, traplistener, MakeParser, MakeLoadConf, MakeOIDConv, MakeSetTest, MakeIpSetTest
#Compiler, wieso nicht standard?
#export CC=gcc
#export LEX=flex
#Loeschen
#export RM=rm
#Ausgabe
export ECHO=echo
#Verzeichnisse
export IpDir=ipstuff
export ParserDir=parserstuff
export ConfDir=confstuff
export MysqlDir=mysqlstuff
export CleanDirs=$(IpDir) $(ParserDir) $(ConfDir) $(MysqlDir) 
#Include-Verzeichnisse
export Include=-I. -I../$(IpDir) -I../$(ParserDir) -I../$(ConfDir) -I../$(MysqlDir)
#Warnings & Co
export CFlags=-g -W -Wall
#Fuer den Linker
export CLFcrypto=-lssl -lcrypto
export CLFmysql=-lmysqlclient
export CLFflex=-lfl
#Compilepath
export Comppath=~/exec/
#.o Dateien, die geloescht werden sollen
export Del_Obj_Files=*.o
#.c Dateien, die geloescht werden sollen
export Del_C_Files=confscan.c

.phony: MakeParser MakeLoadConf MakeOIDConv MakeSetTest MakeIpSetTest MakeIpConv

ALL: clean traplistener

traplistener: traplistener.o #confparse.o confscan.o confstructs.o
	$(CC) $^ -o $(Comppath)$@ $(Include) $(CFlags) $(CLFcrypto) $(CLFmysql) $(CLFflex)

traplistener.o: traplistener.c
	$(CC) -c $< -o $@ $(Include) $(CFlags) #$(CDirFlags)

MakeIpConv: clean
	cd $(IpDir); $(MAKE) ipconv

MakeIpSetTest: clean #Hier koennte man auch auf ein weiteres Target referieren
	cd $(ParserDir); $(MAKE) confscan.o confparse.o
	cd $(ConfDir); $(MAKE) constructs.o 
	cd $(IpDir); $(MAKE) ipsetstruct

MakeSetTest: clean setstruct ;
#Empty Rule

MakeOIDConv: clean oidconv ;
#Empty Rule

MakeLoadConf: clean loadconf ;
#Empty Rule

MakeParser: clean confparse ;
#Empty Rule

clean:
	$(RM) -f $(Del_Obj_Files)
	for cleandir in $(CleanDirs); do (cd $$cleandir; $(MAKE) clean); done

testvar:
	@$(ECHO) "Printing all Variables"
	#@$(ECHO) ECHO: $(ECHO)
	#@$(ECHO) CC: $(CC)
	#@$(ECHO) LEX: $(LEX)
	#@$(ECHO) RM: $(RM)
	#@$(ECHO) IpDir: $(IpDir)
	#@$(ECHO) ParserDir: $(ParserDir)
	#@$(ECHO) ConfDir: $(ConfDir)
	#@$(ECHO) MysqlDir: $(MysqlDir)
	#@$(ECHO) CleanDirs: $(CleanDirs)
	#@$(ECHO) Include: $(Include)
	#@$(ECHO) CFlags: $(CFlags)
	#@$(ECHO) CLFcrypto: $(CLFcrypto)
	#@$(ECHO) CLFmysql: $(CLFmysql)
	#@$(ECHO) CLFflex: $(CLFflex)
	#@$(ECHO) Comppath: $(Comppath)
	#@$(ECHO) Del_Obj_Files: $(Del_Obj_Files)
	#@$(ECHO) Del_C_Files: $(Del_C_Files)

