/***
 * Zu benutzen mit netcat!
 * netcat -u testpc 32000 oder so...
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <arpa/inet.h>

const int packetsize=1500;

int main (int argc, char *argv[]) {
	int port;
	if(argc<2){
		printf("Zu wenig Argumente\n\n");
		return 1;
	}
	port=atoi(argv[1]);
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
	server_address.sin6_addr=in6addr_any;//Hier sollte nur unser SUBNETZ rein!!!
	server_address.sin6_port=htons(port);
	address_length=sizeof(server_address);
	if(bind(server_socky,(struct sockaddr*)&server_address,address_length)==-1){
		perror("Bind");//Unlink!!
		return 1;//(EXIT_FAILURE);
	}
	//Addresse des Empfaengers
	client_address.sin6_family=AF_INET6;
	client_address.sin6_addr=in6addr_any;
	client_address.sin6_port=htons(port);//Noetig?
	//Empfange...
	address_length=sizeof(client_address);
	while(1){
		if(recvfrom(server_socky,trap,packetsize,0,(struct sockaddr *)&client_address,&address_length)==-1){
			perror("Read");
		}
		inet_ntop(AF_INET6,(struct in_addr *)&client_address.sin6_addr,addresse,INET6_ADDRSTRLEN);
		printf("Client:\nFamily: %hd ; Port: %hu ; Addresse: %s\n",client_address.sin6_family,client_address.sin6_port,addresse);
		int i;
		for(i=0;(i<packetsize)&&(trap[i]!='\n');i++)putchar(trap[i]);
		puts("");
		if(trap[0]=='\n')break;
	}
	close(server_socky);
	return 0;
}
