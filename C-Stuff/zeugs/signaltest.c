#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

//Signale stehen in /usr/include/asm/signal.h
//SIGHUP,SIGINT,SIGQUIT,SIGILL,SIGTRAP,SIGABRT,SIGIOT,SIGBUS,SIGFPE,SIGKILL,SIGUSR1,
//SIGSEGV,SIGUSR2,SIGPIPE,SIGALRM,SIGTERM,SIGSTKFLT,SIGCHLD,SIGCONT,SIGSTOP,SIGTSTP,
//SIGTTIN,SIGTTOU,SIGURG,SIGXCPU,SIGXFSZ,SIGVTALRM,SIGPROF,SIGWINCH,SIGIO,SIGPOLL,
//SIGIO,/*SIGLOST,*/SIGPWR,SIGSYS,SIGUNUSED

volatile int gotSignal;

void getSignal(int sig){
	gotSignal=sig;
}

int main(int argc,char*argv[]){
	(void)signal(SIGHUP,getSignal);
	(void)signal(SIGINT,getSignal);
	(void)signal(SIGQUIT,getSignal);
	(void)signal(SIGILL,getSignal);
	(void)signal(SIGTRAP,getSignal);
	(void)signal(SIGABRT,getSignal);
	(void)signal(SIGIOT,getSignal);
	(void)signal(SIGBUS,getSignal);
	(void)signal(SIGFPE,getSignal);
	(void)signal(SIGKILL,getSignal);
	(void)signal(SIGUSR1,getSignal);
	(void)signal(SIGSEGV,getSignal);
	(void)signal(SIGUSR2,getSignal);
	(void)signal(SIGPIPE,getSignal);
	(void)signal(SIGALRM,getSignal);
	(void)signal(SIGTERM,getSignal);
	(void)signal(SIGSTKFLT,getSignal);
	(void)signal(SIGCHLD,getSignal);
	(void)signal(SIGCONT,getSignal);
	(void)signal(SIGSTOP,getSignal);
	(void)signal(SIGTSTP,getSignal);
	(void)signal(SIGTTIN,getSignal);
	(void)signal(SIGTTOU,getSignal);
	(void)signal(SIGURG,getSignal);
	(void)signal(SIGXCPU,getSignal);
	(void)signal(SIGXFSZ,getSignal);
	(void)signal(SIGVTALRM,getSignal);
	(void)signal(SIGPROF,getSignal);
	(void)signal(SIGWINCH,getSignal);
	(void)signal(SIGIO,getSignal);
	(void)signal(SIGPOLL,getSignal);
	(void)signal(SIGIO,getSignal);

	#define SIGLOST 29

	(void)signal(SIGLOST,getSignal);//29
	(void)signal(SIGPWR,getSignal);
	(void)signal(SIGSYS,getSignal);
	(void)signal(SIGUNUSED,getSignal);
	gotSignal=0;
	printf("%d\n",getpid());
	while(gotSignal!=SIGTERM){
		printf("GotSignal: %d\n",gotSignal);
		pause();
	}
	printf("GotSignal: %d\n",gotSignal);
	return EXIT_SUCCESS;
}
