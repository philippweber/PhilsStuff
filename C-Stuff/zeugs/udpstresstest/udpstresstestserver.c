#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
//#include <sys/time.h>

#define packetsize 65536

int main(int argc,char*argv[]){
	if(argc<2){
		fprintf(stderr,"Bitte Port angeben\n");
		return 1;
	}
	int port;
	if((port=atoi(argv[1]))==0){
		fprintf(stderr,"Kann Port nicht konvertieren\n");
		return 1;
	}
	printf("Port: %d\n",port);
	int server_socky;//Server-Socket
	socklen_t address_length;//Addresslaenge
	unsigned char trap[packetsize];//Packetbuffer
	char addresse[INET6_ADDRSTRLEN];//Adresse
	struct sockaddr_in6 server_address,client_address;
	//Server einrichten
	if((server_socky=socket(AF_INET6,SOCK_DGRAM,0))==-1){
		perror("Socket");
		return 1;//(EXIT_FAILURE);
	}
	server_address.sin6_family=AF_INET6;
	server_address.sin6_addr=in6addr_any;
	server_address.sin6_port=htons(port);
	address_length=sizeof(server_address);
	if(bind(server_socky,(struct sockaddr*)&server_address,address_length)==-1){
		perror("Bind");
		close(server_socky);
		return 1;//(EXIT_FAILURE);
	}
	//Addresse des Empfaengers
	client_address.sin6_family=AF_INET6;
	client_address.sin6_addr=in6addr_any;
	client_address.sin6_port=htons(port);//Noetig?
	address_length=sizeof(client_address);
	//Empfange...
	int recvsize;
	long long int packet_counter=0;
	while(1){
		if((recvsize=recvfrom(server_socky,trap,packetsize,0,(struct sockaddr *)&client_address,&address_length))==-1){
			perror("Read");
		}else{
			packet_counter++;
			//printf("%d\n",recvsize);
			//if((packet_counter&&0xFFFF)==0)printf("Recieved: %llu",packet_counter);
			if((packet_counter%0x10000)==0)printf("Recieved: %llu\n",packet_counter);
			if(recvsize!=5){
				printf("Recieved: %llu\n",packet_counter);
				inet_ntop(AF_INET6,(struct in_addr *)&client_address.sin6_addr,addresse,INET6_ADDRSTRLEN);
				printf("Client:\nFamily: %hd ; Port: %d ; Addresse: %s\n",client_address.sin6_family,client_address.sin6_port,addresse);
				printf("%.*s\n",recvsize,trap);
			}
		}
	}
	close(server_socky);
	return 0;
}
