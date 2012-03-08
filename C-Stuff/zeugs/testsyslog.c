#include <stdio.h>
#include <syslog.h>
int main(int argc,char*argv[]){
	if(argc<2){puts("Zu wenig Argumente");return -1;}
	openlog(argv[0],LOG_CONS|LOG_PERROR|LOG_PID,LOG_LOCAL7);
	syslog(LOG_NOTICE,"Program started by User %d",getuid());
	syslog(LOG_EMERG,argv[1]);
	closelog();
	return 0;
}
