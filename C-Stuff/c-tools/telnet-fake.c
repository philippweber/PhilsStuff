#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/fcntl.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>

#define maxpacketsize 0x10000
#define telnetport 0x1700 //==htons(IPPORT_TELNET)==htons(23)

void ipv4_to_ipv6(const struct in_addr*IPv4,struct in6_addr*IPv6){
	memset(IPv6,0,sizeof(struct in6_addr));
	IPv6->s6_addr32[3]=IPv4->s_addr;
	IPv6->s6_addr16[5]=(uint16_t)0xFFFF;
}

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
	if(argc<2){
		fprintf(stderr,"IP-Adresse angeben\n");
		return 1;
	}
	//Daten
	int socky;
	long long connect_counter;
	socklen_t address_length;
	struct in6_addr*switch_address_pure;
	struct sockaddr_in6 switch_address;
	//Adresse des Switches
	if((switch_address_pure=ipv6_inet_pton(argv[1]))==NULL){
		fprintf(stderr,"IP-Adresse ungueltig\n");
		return 1;
	}
	//Struktur vorbereiten
	memset(&switch_address,0,sizeof(switch_address));//==sizeof(struct sockaddr_in6)
	switch_address.sin6_family=AF_INET6;
	memcpy(&switch_address.sin6_addr,switch_address_pure,sizeof(struct in6_addr));
	switch_address.sin6_port=telnetport;//htons(23);htons(IPPORT_TELNET);
	address_length=sizeof(switch_address);
	free(switch_address_pure);
	//Endlosschleife
	for(connect_counter=1;1;connect_counter++){
		//#define failactionbeforecreation return 1;
		//#define failaction failactionbeforecreation
		#define failactionbeforecreation continue;
		#define failaction close(socky);failactionbeforecreation
		printf("Verbindung '%lld'\n",connect_counter);
		//Socket erzeugen
		if((socky=socket(AF_INET6,SOCK_STREAM,0))==-1){
			perror("Socket: ");
			failactionbeforecreation
		}
		if(connect(socky,(struct sockaddr*)&switch_address,address_length)!=0){//Geht auch fuer sockaddr_in6
			perror("Connect: ");
			failaction
		}
		//Uebermittle Daten...
		char recvbuf[maxpacketsize];
		//ssize_t sendlen,recvlen;
		//Definiere Makros,liessen sich auch als Funktionen definieren...
		#define philssend(msg,msglen) \
		if(send(socky,msg,msglen,0)!=msglen){\
			perror("Send: ");\
			failaction\
		}
		#define philsrecv(msg) \
		{\
			if(recv(socky,recvbuf,maxpacketsize,0)<0){\
				perror("Recv: ");\
				return 1;\
			}\
		}while(strstr(recvbuf,msg));
		/*
		if(strncmp(msg,recvbuf,msglen)!=0){\
			fprintf(stderr,"Vergleichsfehler von '%s' und '%.*s'\n",msg,(int)msglen,recvbuf);\
			failaction;\
		}\
		*/
		//Sende los
		#define passwdmsg "Password:"
		philsrecv(passwdmsg)
		#define passwderror "\n"
		philssend(passwderror,sizeof(passwderror))
		//Mache nachher wieder einen auf
		close(socky);
	}
	//Kann nicht passieren
	close(socky);
	return EXIT_SUCCESS;
}
