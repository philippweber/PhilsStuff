//#define _GNU_SOURCE//Dann kann man prlimit() nutzen
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/resource.h>

int main(int argc,char*argv[]){
	struct rlimit old,new;
	//struct rlimit64 old,new;//#ifdef __USE_LARGEFILE64
	//pid_t pid;

	//if(!((argc==2)||(argc==4))){
		//fprintf(stderr, "Usage: <pid> [<new-soft-limit> <new-hard-limit>]\n", argv[0]);
	if(!((argc==1)||(argc==3))){
		fprintf(stderr, "Usage: [<new-soft-limit> <new-hard-limit>]\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	printf("%d\n",getpid());

	//if(getrlimit64(RLIMIT_NPROC,&old)==-1){//#ifdef __USE_LARGEFILE64
	if(getrlimit(RLIMIT_NPROC,&old)==-1){
		perror("getrlimit");
		return 1;
	}
	printf("Previous limits: soft=%lld; hard=%lld\n",
				(long long)old.rlim_cur,(long long)old.rlim_max);

	/*if((pid=atoi(argv[1]))==0){
		pid=getpid();
	}*/

	/*if(prlimit(pid,RLIMIT_NPROC,NULL,&old)==-1){
		perror("prlimit");
		return 1;
	}
	printf("Previous limits: soft=%lld; hard=%lld\n",
				(long long)old.rlim_cur,(long long)old.rlim_max);*/

	//if(argc!=4)return 0;
	//new.rlim_cur=atoi(argv[2]);
	//new.rlim_max=atoi(argv[3]);
	if(argc!=3)return 0;
	new.rlim_cur=atoi(argv[1]);
	new.rlim_max=atoi(argv[2]);

	/*if(prlimit(pid,RLIMIT_NPROC,&new,&old)==-1){
		perror("prlimit");
		return 1;
	}
	printf("Previous limits: soft=%lld; hard=%lld\n",
				(long long)old.rlim_cur,(long long)old.rlim_max);
	printf("New limits: soft=%lld; hard=%lld\n",
				(long long)new.rlim_cur,(long long)new.rlim_max);*/

	//if(setrlimit64(RLIMIT_CPU,&new)==-1){//#ifdef __USE_LARGEFILE64
	if(setrlimit(RLIMIT_NPROC,&new)==-1){
		perror("setrlimit");
		return 1;
	}
	printf("New limits: soft=%lld; hard=%lld\n",
				(long long)new.rlim_cur,(long long)new.rlim_max);

	//Fuers Testen
	//pause();

	return 0;
}
