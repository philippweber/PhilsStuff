/***
 * Diese Programm ersetzt
 * echo -ne "????\0" | netcat -uq 5 ???? ????
 * d.h. es schickt eine Testnachricht an einen Port und erwartet die Antwort!
 * Alt-Skript: for port in `seq 65531 1 65534`; do echo -ne "Welt\0" | netcat -q 0 -u -p $port localhost 42420; done
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

#define port_checker_EXIT_FAILURE 1
#define port_checker_debug 1

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
	if(argc<6){
		#ifdef port_checker_debug
			fprintf(stderr,"Bitte Frage, Antwort, Listeningport, Adresse und Ports angeben\n");
		#endif
		return port_checker_EXIT_FAILURE;
	}
	int listenport;
	if((listenport=atoi(argv[3]))==0){
		#ifdef port_checker_debug
			fprintf(stderr,"Kann Port '%s' nicht konvertieren\n",argv[3]);
		#endif
		return port_checker_EXIT_FAILURE;
	}
	size_t argv1len=strlen(argv[1])+1;//Laenge der Frage
	size_t argv2len=strlen(argv[2])+1;//Laenge der Antwort
	struct in6_addr*ip_addr=ipv6_inet_pton(argv[4]);
	if(ip_addr==NULL){
		#ifdef port_checker_debug
			fprintf(stderr,"Kann IP-Adresse nicht konvertieren\n");
		#endif
		return port_checker_EXIT_FAILURE;
	}
	int i;
	int res=EXIT_SUCCESS;
	for(i=5;i<argc;i++){
		int port;
		if((port=atoi(argv[i]))==0){
			#ifdef port_checker_debug
				fprintf(stderr,"Kann Port '%s' nicht konvertieren\n",argv[i]);
			#endif
			res=port_checker_EXIT_FAILURE;
			continue;
		}
		#ifdef port_checker_debug
			printf("Port: %d\n",port);
		#endif
		//Socket einrichten
		int socky;//Client-Socket
		if((socky=socket(AF_INET6,SOCK_DGRAM,0))==-1){
			#ifdef port_checker_debug
				perror("Socket");
			#endif
			res=port_checker_EXIT_FAILURE;
			continue;
		}
		fcntl(socky,F_SETFL,O_NONBLOCK);
		//Addresse des Senders, setze jedesmal neu, naja...
		socklen_t address_length;//Addresslaenge
		struct sockaddr_in6 server_address,client_address;
		client_address.sin6_family=AF_INET6;
		//memcpy(&client_address.sin6_addr,ip_addr,sizeof(struct in6_addr));
		client_address.sin6_addr=in6addr_any;
		client_address.sin6_port=htons(listenport);
		address_length=sizeof(client_address);
		if(bind(socky,(struct sockaddr*)&client_address,address_length)==-1){
			#ifdef port_checker_debug
				perror("Bind");
			#endif
			res=port_checker_EXIT_FAILURE;
			close(socky);
			continue;
		}
		//Addresse des Empfaengers, setze jedesmal neu, naja...
		server_address.sin6_family=AF_INET6;
		memcpy(&server_address.sin6_addr,ip_addr,sizeof(struct in6_addr));
		server_address.sin6_port=htons(port);
		server_address.sin6_flowinfo=0;
		server_address.sin6_scope_id=0;
		address_length=sizeof(server_address);
		//Sende
		#ifdef port_checker_debug
			printf("Sende '%s' an '%d'\n",argv[1],port);
		#endif
		if(sendto(socky,argv[1],argv1len,0,(struct sockaddr*)&server_address,address_length)!=argv1len){
			#ifdef port_checker_debug
				perror("Send");
			#endif
			res=port_checker_EXIT_FAILURE;
			continue;
		}
		//Warten
		#ifdef port_checker_debug
			printf("Warte\n");
		#endif
		sleep(5);
		//Empfangen
		size_t recvsize,packetsize=0x10000;
		char buffer[0x10001];//64K==MaxSize
		if((recvsize=recvfrom(socky,buffer,packetsize,0,(struct sockaddr*)&server_address,&address_length))==-1){
			#ifdef port_checker_debug
				perror("Read");
			#endif
			close(socky);
			res=port_checker_EXIT_FAILURE;
			continue;
		}
		//Wird ab jetzt nicht mehr benoetigt
		close(socky);
		if(server_address.sin6_port!=htons(port)){
			#ifdef port_checker_debug
				fprintf(stderr,"Falscher Port '%d' / '%d'\n",port,ntohs(server_address.sin6_port));
			#endif
			res=port_checker_EXIT_FAILURE;
			continue;
		}
		if(recvsize!=argv2len){
			#ifdef port_checker_debug
				fprintf(stderr,"Falsche Laenge an Port '%d': '%d' zu '%d'\n",port,argv2len,recvsize);
			#endif
			res=port_checker_EXIT_FAILURE;
			continue;
		}
		if(memcmp(buffer,argv[2],argv2len)!=0){
			#ifdef port_checker_debug
				fprintf(stderr,"Fehlerhafte Strings an Port '%d': '%s' zu '%.*s'\n",port,argv[2],recvsize,buffer);
			#endif
			res=port_checker_EXIT_FAILURE;
			continue;
		}
		#ifdef port_checker_debug
			printf("Erfolgreicher Test an Port '%d': '%s' zu '%.*s'\n",port,argv[2],recvsize,buffer);
		#endif
	}
	free(ip_addr);
	return EXIT_SUCCESS;
}
