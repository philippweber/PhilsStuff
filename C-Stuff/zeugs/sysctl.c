#define _GNU_SOURCE
#include <unistd.h>
#include <sys/syscall.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <linux/sysctl.h>

int _sysctl(struct __sysctl_args *args );

#define OSNAMESZ 100

int main(void){
	struct __sysctl_args args;
	char osname[OSNAMESZ];
	size_t osnamelth;
	int name[]={CTL_KERN,KERN_OSTYPE};
	memset(&args,0,sizeof(struct __sysctl_args));//Auf Null setzen!
	//Stattdessen koennte man auch:
	//args.newval=NULL;args.newlen=0;
	//Rest wird gesetzt!
	args.name=name;//Befehle
	args.nlen=sizeof(name)/sizeof(name[0]);//Auf Integer gerechnet
	args.oldval=osname;
	args.oldlenp=&osnamelth;
	osnamelth=sizeof(osname);
	if(syscall(SYS__sysctl,&args)==-1){
		perror("_sysctl");
		exit(EXIT_FAILURE);
	}
	printf("This machine is running %*s\n",osnamelth,osname);

	//Teste rmem_default und rmem_max
	//Zuerst rmem_default
	int rmem_default[]={CTL_NET,NET_CORE,NET_CORE_RMEM_DEFAULT};
	int buffer[2];//Doppelte Groesse, sicherheitshalber
	size_t buffersize=sizeof(buffer);
	memset(&args,0,sizeof(struct __sysctl_args));//Auf Null setzen, sicherheitshalber nochmals!
	//Stattdessen koennte man auch:
	//args.newval=NULL;args.newlen=0;
	//Rest wird gesetzt!
	args.name=rmem_default;
	args.nlen=sizeof(rmem_default)/sizeof(rmem_default[0]);//Auf Integer gerechnet
	args.oldval=buffer;
	args.oldlenp=&buffersize;
	if(syscall(SYS__sysctl,&args)==-1){
		perror("_sysctl");
		exit(EXIT_FAILURE);
	}
	printf("Size %d , rmem_default %d\n",buffersize,buffer[0]);

	//Teste rmem_max
	int rmem_max[]={CTL_NET,NET_CORE,NET_CORE_RMEM_MAX};
	buffersize=sizeof(buffer);
	memset(&args,0,sizeof(struct __sysctl_args));//Auf Null setzen, sicherheitshalber nochmals!
	//Stattdessen koennte man auch:
	//args.newval=NULL;args.newlen=0;
	//Rest wird gesetzt!
	args.name=rmem_max;
	args.nlen=sizeof(rmem_max)/sizeof(rmem_max[0]);//Auf Integer gerechnet
	args.oldval=buffer;
	args.oldlenp=&buffersize;
	if(syscall(SYS__sysctl,&args)==-1){
		perror("_sysctl");
		exit(EXIT_FAILURE);
	}
	printf("Size %d , rmem_max %d\n",buffersize,buffer[0]);

	exit(EXIT_SUCCESS);
}
