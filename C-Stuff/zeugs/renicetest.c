#include<stdio.h>
#include<unistd.h>
#include<sys/resource.h>
#include<errno.h>
int main(int argc,char*argv[]){
	if(argc<2){
		fprintf(stderr,"Bitte geben Sie die neue Nice an\n");
		return 1;
	}
	int newnice=atoi(argv[1]),pid=getpid();
	printf("Von %d zu %d\n",getpriority(PRIO_PROCESS,pid),newnice);
	if(setpriority(PRIO_PROCESS,pid,newnice)!=0){
		printf("%d: %s\n",errno,strerror(errno));
		//perror("setpriority");
	}
	printf("Jetzt %d (ps l)\n",getpriority(PRIO_PROCESS,pid));
	sleep(10);
	return 0;
}
