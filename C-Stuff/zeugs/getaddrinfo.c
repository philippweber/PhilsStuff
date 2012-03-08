#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>

int main(int argc,char*argv[]){
	struct addrinfo*infos,keys;
	int result;
	if(argc<2){
		fprintf(stderr,"Bitte geben Sie einen Hostnamen an\n");
		exit(1);
	}
	//Holen
	keys.ai_family=AF_UNSPEC;//AF_INET6 um IPv6 zu erzwingen
	keys.ai_socktype=0;//SOCK_DGRAM;SOCK_STREAM;
	if((result=getaddrinfo(argv[1],NULL,&keys,&infos))!= 0){
		fprintf(stderr,"Getaddrinfo: '%s'\n",gai_strerror(result));
		exit(2);
	}
	//Ausgeben
	struct addrinfo*p;
	char ipv6text[INET6_ADDRSTRLEN];//IPv6, kann auch IPv4
	for(p=infos;p!=NULL;p=p->ai_next){
		inet_ntop(p->ai_family,
				(p->ai_family==AF_INET6)?((void*)&((struct sockaddr_in6*)p->ai_addr)->sin6_addr):((void*)&((struct sockaddr_in*)p->ai_addr)->sin_addr),
				ipv6text,INET6_ADDRSTRLEN);
		printf("'%s'\n",ipv6text);
		/*int i;
		for(i=0;i<p->ai_addrlen;i++){
			printf("%02hX ",(char)((&p->ai_addr)[i])&0xFF);
		}
		puts("");*/
	}
	freeaddrinfo(infos);
	return EXIT_FAILURE;
}

