#include<sys/types.h>
#include<stdio.h>
#include<unistd.h>
#include<errno.h>

int main(int argc,char*argv[]){
	if(argc<2){
		fprintf(stderr,"Bitte Datei angeben.\n");
		return 1;
	}
	if(access(argv[1],F_OK)!=0){
		if(errno==ENOENT){
			printf("Verzeichnis existiert nicht: '%d' ; '%s'\n",errno,strerror(errno));
		}else{
			perror("access");
			return 1;
		}
	}else{
		printf("Datei existiert.\n");
	}
	return 0;
}
