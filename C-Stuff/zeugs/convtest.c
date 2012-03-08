#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/***
 * Verwandelt eine Zahl in einem String in eine binaer Zahl
 * Benoetigt den Textstring und einen Pointer auf einen unsigned int (der zur Rueckgabe dient)
 * Rueckgabe des Fehlers, 0 sonst
 */
char convert_uint(char*numtext,unsigned int*num){
	unsigned long res;
	errno=0;
	res=strtoul(numtext,NULL,10);

	printf("%lu\n",res);

	if(res>UINT_MAX)return ERANGE;/*Ausser Reichweite!*/
	*num=(unsigned int)res;
	return errno;
}

int main(int argc, char*argv[]){
	if(argc<2){fprintf(stderr,"Falsche Anzahl Parameter: Wert\n");return 1;}
	unsigned int ui;
	#ifdef UINT_MAX
		printf("UINT_MAX: %u\n",UINT_MAX);
	#endif
	if(!convert_uint(argv[1],&ui))printf("Typ: Unsigned Int, Wert: %u\n",ui);
	else printf("Typ: Unsigned Int, Wert: Ungueltig\n");
}
