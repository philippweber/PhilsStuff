/***
 * Diese Programm wurde zum Testen des Verhaltens von dlopen/dlclose waehrend eines Forks geschrieben
 * "ln -sf libmodule1.so libmodule.so"
 * Man startet das Programm mit libmodule.so
 * Es laedt die Bib und forkt, dann startet es in beiden die Bib-Funktion "module_start"
 * Es wartet im Parent 5 Sek. bis es die Bib entlaedt und neu laedt
 * Davor sollte ein "ln -sf libmodule2.so libmodule.so" gemacht sein
 * Es wartet im Child 10 Sek. bis es nochmal 10-mal die Bib-Funktion "module_start" ausfuehrt
 * Geht dies?
 */

#include <stdio.h>
#include <dlfcn.h>
#include <ctype.h>
#include <unistd.h>
#include  <sys/types.h>

#include "module.h"

int main(int argc,char*argv[])
{
	if(argc<2){fprintf(stderr,"Bitte geben Sie eine Bibliothek an...\n");return 2;}
	void*lib_handle;
	void(*func_ptr)();
	char*error;
	pid_t pid,fork_pid;
	//Oeffne erste Bib
	lib_handle=dlopen(argv[1],RTLD_LAZY);
	if(!lib_handle){fprintf(stderr,"%s\n",dlerror());return 1;}
	if((func_ptr=dlsym(lib_handle,"module_start"))==NULL){fprintf(stderr,"%s\n",dlerror());return 1;}
	if((error=dlerror())!=NULL){fprintf(stderr,"%s\n",error);return 1;}
	//Forke, schliesse Bib im Parent
	pid=getpid();
	fork_pid=fork();
	if(fork_pid<0){fprintf(stderr,"FORK-FEHLER\n");return 1;}
	if(fork_pid==0){
		(*func_ptr)();
		sleep(10);
		int i;
		for(i=0;i<10;i++){(*func_ptr)();sleep(1);}
	}else{
		(*func_ptr)();
		sleep(5);
		dlclose(lib_handle);
		//Oeffne Bib nochmal
		lib_handle=dlopen(argv[1],RTLD_LAZY);
		if(!lib_handle){fprintf(stderr,"%s\n",dlerror());return 1;}
		if((func_ptr=dlsym(lib_handle,"module_start"))==NULL){fprintf(stderr,"%s\n",dlerror());return 1;}
		if((error=dlerror())!=NULL){fprintf(stderr,"%s\n",error);return 1;}
		(*func_ptr)();
		//Warte auf Kind...
		waitpid(-1,NULL,0);
		(*func_ptr)();
		dlclose(lib_handle);
	}
	return 0;
}
