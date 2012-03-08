#include <limits.h> //PATH_MAX
#include <stdio.h>
#include <stdlib.h>
int main(int argc,char*argv[]){
	if(argc<2){
		fprintf(stderr,"Bitte Dateinamen angeben.\n");
		return 1;
	}
	char buf[PATH_MAX+1];
	char*res=realpath(argv[1],buf);
	if(res!=NULL){
		printf("Vollstaendiger Pfadname '%s' (PATH_MAX+1:'%d').\n",buf,PATH_MAX+1);
	}else{
		perror("realpath");
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
