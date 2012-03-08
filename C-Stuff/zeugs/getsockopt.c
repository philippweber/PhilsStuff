#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

int main(int argc,char*argv[]){
	if(argc<2){
		fprintf(stderr,"Bitte geben Sie einen Port an\n");
		return 1;
	}
	int port;
	if((port=atoi(argv[1]))==0){
		fprintf(stderr,"Kann Port nicht konvertieren\n");
		return 1;
	}
	int socky;
	//Erzeugen
	if((socky=socket(AF_INET6,SOCK_DGRAM,0))==-1){
			perror("Socket");
			//Hier fehlt das Aufraeumen!!!
			return 1;//(EXIT_FAILURE);
	}
	//Binden
	/*struct sockaddr_in6 server_address;
	server_address.sin6_family=AF_INET6;
	server_address.sin6_addr=in6addr_any;
	//socklen_t address_length=sizeof(server_address);//Addresslaenge
	server_address.sin6_port=htons(port);
	if(bind(socky,(struct sockaddr*)&server_address,sizeof(server_address))==-1){
		perror("Bind");//Unlink!!
		//Hier fehlt das Aufraeumen!!!
		return 1;//(EXIT_FAILURE);
	}*/
	//Los gehts
	char optbuffer[1024];
	int optlen,i;
	#define testOption(optionToTest) puts(#optionToTest);\
		optlen=sizeof(optbuffer);\
		for(i=0;i<optlen;i++)optbuffer[i]=0;\
		if(getsockopt(socky,SOL_SOCKET,optionToTest,optbuffer,&optlen)!=0){\
			perror("Getsockopt");\
		}\
		printf("%d\n",*((int*)optbuffer));\
		for(i=0;i<optlen;i++){\
			printf("%2hX ",optbuffer[i]&0xFF);\
		}\
		puts("");
	testOption(SO_DEBUG)
	testOption(SO_ACCEPTCONN)
	testOption(SO_BROADCAST)
	testOption(SO_REUSEADDR)
	testOption(SO_KEEPALIVE)
	testOption(SO_LINGER)
	testOption(SO_OOBINLINE)
	testOption(SO_SNDBUF)
	testOption(SO_RCVBUF)
	testOption(SO_ERROR)
	testOption(SO_TYPE)
	testOption(SO_DONTROUTE)
	testOption(SO_RCVLOWAT)
	testOption(SO_RCVTIMEO)
	testOption(SO_SNDLOWAT)
	testOption(SO_SNDTIMEO)
	close(socky);
}
