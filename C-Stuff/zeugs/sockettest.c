#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/fcntl.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

#define MaxPacketSize 65536

struct portliststruct{
	struct portliststruct*next;
	int port;
	int socket;
};

int main(int argc,char*argv[]){
	if(argc<3){
		fprintf(stderr,"Bitte geben Sie mindestens einen Port an (max. %d)\n",FD_SETSIZE);
		return 1;//(EXIT_FAILURE);
	}
	//Alle Ports holen
	int maxports=0,i,p;//maxports==Anzahl Ports
	struct portliststruct*head=NULL,*next;
	for(i=1;i<argc;i++,maxports++){
		p=atoi(argv[i]);
		if((p>65535)||(p<1)){
			fprintf(stderr,"Port kann nicht uebersetzt werden, da keine Zahl, zu gross oder zu klein\n");
			//Hier fehlt das Aufraeumen!!!
			return 1;//(EXIT_FAILURE);
		}
		next=malloc(sizeof(struct portliststruct));
		next->port=p;
		next->next=head;
		head=next;
	}
	//FD_SET initialisieren
	int maxsocket=0;//maxsocket==Hoechster Socketwert
	fd_set port_set;
	FD_ZERO(&port_set);
	int opts;
	//Anfangen mit Sockets fuer Ports oeffnen
	struct sockaddr_in6 server_address;
	server_address.sin6_family=AF_INET6;
	server_address.sin6_addr=in6addr_any;
	//server_address.sin6_port=htons(port);
	for(next=head;next!=NULL;next=next->next){
		//Erzeugen
		if((next->socket=socket(AF_INET6,SOCK_DGRAM,0))==-1){
			perror("Socket");
			//Hier fehlt das Aufraeumen!!!
			return 1;//(EXIT_FAILURE);
		}
		//Binden
		//socklen_t address_length=sizeof(server_address);//Addresslaenge
		server_address.sin6_port=htons(next->port);
		if(bind(next->socket,(struct sockaddr*)&server_address,sizeof(server_address))==-1){
			perror("Bind");//Unlink!!
			//Hier fehlt das Aufraeumen!!!
			return 1;//(EXIT_FAILURE);
		}
		opts=fcntl(next->socket,F_GETFL);
		if(opts<0){
			perror("fcntl(F_GETFL)");
			exit(EXIT_FAILURE);
		}
		opts=(opts|O_NONBLOCK);
		if(fcntl(next->socket,F_SETFL,opts)<0){
			perror("fcntl(F_SETFL)");
			exit(EXIT_FAILURE);
		}
		//Hinzufuegen
		FD_SET(next->socket,&port_set);
		if(next->socket>maxsocket)maxsocket=next->socket;
	}
	int quit;//Programm beenden?
	while(quit==0){
		int sel_res;//,desc_ready;//Select-Resultat==Anzahl Verbindungen
		struct timeval timeout;//Wartezeit, bei Uebergabe von NULL==>Unendliches Blocken
		timeout.tv_sec=3*60;
		timeout.tv_usec=0;
		sel_res=select(maxsocket+1,&port_set,NULL,NULL,&timeout);
		//Auf Fehler pruefen
		if(sel_res<0){
			perror("select() failed");
			break;
		}
		//Zeit abgelaufen
		if(sel_res==0){
			printf("select() timed out.  End program.\n");
			break;
		}
		//Wieviele Deskriptoren sind bereit?
		//desc_ready=sel_res;
		//Welche Deskriptoren sind bereit?
		//for(i=0,next=head;(i<=maxports)&&(desc_ready>0)&&(next!=NULL);i++,next=next->next){
		//So kann ich gleichzeitig das fd_set reseten!
		for(next=head;next!=NULL;next=next->next){
			printf("Teste %d: ",next->port);
			//Deskriptor bereit?
			if(FD_ISSET(next->socket,&port_set)){
				printf("Bereit: %d\n",next->port);
				//Einer gefunden, nach einem weniger suchen
				//desc_ready--;
				int packetsize;
				char addresse[INET6_ADDRSTRLEN];//Adresse als Text
				struct sockaddr_in6 client_address;//Adresse
				int address_length=sizeof(client_address);//Man muss Pointer uebergeben
				char packetbuffer[MaxPacketSize];//Buffer
				if((packetsize=recvfrom(next->socket,packetbuffer,MaxPacketSize,0,(struct sockaddr *)&client_address,&address_length))==-1){
					if(errno!=EWOULDBLOCK){
						perror("Read");
						quit=1;
						break;
					}
				}
				//Daten verfuegbar
				if(packetsize>0){
					inet_ntop(AF_INET6,(struct in_addr *)&client_address.sin6_addr,addresse,INET6_ADDRSTRLEN);
					printf("Client:\nFamily: %hd ; Port: %d ; Addresse: %s\n",client_address.sin6_family,client_address.sin6_port,addresse);
					printf("%.*s",packetsize,packetbuffer);
					if(memcmp(packetbuffer,"exit",(packetsize<4)?packetsize:4)==0){
						printf("Beende Programm\n");
						quit=1;
						break;
					}
				}
			}else{
				printf("Nicht bereit: %d\n",next->port);
			}
			FD_SET(next->socket,&port_set);
		}
	}

	//Aufraeumen

}
