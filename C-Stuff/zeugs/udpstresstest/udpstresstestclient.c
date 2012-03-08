#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
//#include <sys/time.h>

/***
 * Macht aus einer IPv4 eine IPv6 Adresse
 * Benoetigt die IPv4-Adresse und Platz fuer IPv6
 */
void ipv4_to_ipv6(const struct in_addr*IPv4,struct in6_addr*IPv6)
{
	memset(IPv6,0,sizeof(struct in6_addr));
	IPv6->s6_addr32[3]=IPv4->s_addr;
	IPv6->s6_addr16[5]=(uint16_t)0xFFFF;
}

/***
 * Wandelt einen String in eine IPv6 Adressstruktur um, falls die Adresse gueltig war
 * Benoetigt die IP Adresse als String
 * Rueckgabe der IP Adresse als IPv6 Adressstruktur
 */
struct in6_addr*ipv6_inet_pton(char*ipstring){
	if(ipstring==NULL)return NULL;
	struct in6_addr*ip=malloc(sizeof(struct in6_addr));//IP-Adresse
	if(!inet_pton(AF_INET6,ipstring,ip)){
		//Es handelt sich nicht um eine IPv6 Adresse!
		struct in_addr ipv4;//Fuer Konvertierung notwendig
		if(!inet_pton(AF_INET,ipstring,&ipv4)){
			//Es handelt sich nicht um eine IPv4 Adresse!
			//Adresse kann nicht konvertiert werden!
			free(ip);
			//parseerrorcounter++;//Fehlerzaehler korrigiert
			return NULL;
		}
		//Wird konvertiert
		ipv4_to_ipv6(&ipv4,ip);
	}
	return ip;
}

int main(int argc,char*argv[]){
	if(argc<3){
		fprintf(stderr,"Bitte Adresse und Port angeben\n");
		return 1;
	}
	struct in6_addr*ip_addr=ipv6_inet_pton(argv[1]);
	if(ip_addr==NULL){
		fprintf(stderr,"Kann IP-Adresse nicht konvertieren\n");
		return 1;//(EXIT_FAILURE);
	}
	int port;
	if((port=atoi(argv[2]))==0){
		fprintf(stderr,"Kann Port nicht konvertieren\n");
		free(ip_addr);
		return 1;
	}
	printf("Port: %d\n",port);
	//Server einrichten
	int client_socky;//Client-Socket
	if((client_socky=socket(AF_INET6,SOCK_DGRAM,0))==-1){
		perror("Socket");
		free(ip_addr);
		return 1;//(EXIT_FAILURE);
	}
	socklen_t address_length;//Addresslaenge
	struct sockaddr_in6 server_address,client_address;
	client_address.sin6_family=AF_INET6;
	client_address.sin6_addr=in6addr_any;
	client_address.sin6_port=htons(port);
	address_length=sizeof(client_address);
	if(bind(client_socky,(struct sockaddr*)&client_address,address_length)==-1){
		perror("Bind");
		free(ip_addr);
		close(client_socky);
		return 1;//(EXIT_FAILURE);
	}
	//Addresse des Empfaengers
	server_address.sin6_family=AF_INET6;
	memcpy(&server_address.sin6_addr,ip_addr,sizeof(struct in6_addr));
	free(ip_addr);
	server_address.sin6_port=htons(port);
	//Empfange...
	address_length=sizeof(server_address);
	while(1){
		if(sendto(client_socky,"Test",5,0,(struct sockaddr*)&server_address,address_length)!=5){
			perror("Send");
		}
	}
	close(client_socky);
	return 0;
}
