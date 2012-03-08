#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <errno.h>

#define SERVER_PORT 12345

#define TRUE 1
#define FALSE 0

int main(int argc,char*argv[]){
	int i,len,rc,on=1;
	int listen_sd,max_sd,new_sd;
	int desc_ready,end_server=FALSE;
	int close_conn;
	char buffer[80];
	struct sockaddr_in addr;
	struct timeval timeout;
	struct fd_set master_set,working_set;
	//Neuer Hauptsocket (als Startsocket)
	listen_sd=socket(AF_INET,SOCK_STREAM,0);
	if(listen_sd<0){
		perror("socket() failed");
		exit(-1);
	}
	//Socket auf Wiederverwendbar umstellen
	rc=setsockopt(listen_sd,SOL_SOCKET,SO_REUSEADDR,(char*)&on,sizeof(on));
	if(rc<0){
		perror("setsockopt() failed");
		close(listen_sd);
		exit(-1);
	}
	//Nicht blockierend
	rc=ioctl(listen_sd,FIONBIO,(char*)&on);
	if(rc<0){
		perror("ioctl() failed");
		close(listen_sd);
		exit(-1);
	}
	//Bind
	memset(&addr,0,sizeof(addr));
	addr.sin_family=AF_INET;
	addr.sin_addr.s_addr=htonl(INADDR_ANY);
	addr.sin_port=htons(SERVER_PORT);
	rc=bind(listen_sd,(struct sockaddr*)&addr,sizeof(addr));
	if(rc<0){
		perror("bind() failed");
		close(listen_sd);
		exit(-1);
	}
	//Listen, 32 einkommende Connections akzeptieren
	rc=listen(listen_sd,32);
	if(rc<0){
		perror("listen() failed");
		close(listen_sd);
		exit(-1);
	}
	//FD_SET initialisieren
	FD_ZERO(&master_set);
	max_sd=listen_sd;
	FD_SET(listen_sd,&master_set);
	//Beende das Programm nach 3 Minuten inaktvitaet
	timeout.tv_sec=3*60;
	timeout.tv_usec=0;
	//Auf Daten warten
	do{
		//Kopiere master_set nach working_set
		memcpy(&working_set,&master_set,sizeof(master_set));
		//Auf Select warten (oder bis Zeit um)
		printf("Waiting on select()...\n");
		rc=select(max_sd+1,&working_set,NULL,NULL,&timeout);
		//Fehler?
		if(rc<0){
			perror("select() failed");
			break;
		}
		//Zeit um?
		if(rc==0){
			printf("select() timed out.  End program.\n");
			break;
		}
		//Welche Deskriptoren sind bereit?
		desc_ready=rc;
		for(i=0;(i<=max_sd)&&(desc_ready>0);++i){
			//Deskriptor bereit
			if(FD_ISSET(i,&working_set)){
				//Einer gefunden, nach einem weniger suchen
				desc_ready--;
				//Ist es der Hauptsocket
				if(i==listen_sd){
					printf("Listening socket is readable\n");
					//Alle wartenden, einkommenden Verbindungen akzeptieren
					do{
						//Bei EWOULDBLOCK alle Daten erhalten (keine neuen Verbindungen)
						//Bei allen anderen Fehlern==>Ende
						new_sd=accept(listen_sd,NULL,NULL);
						if(new_sd<0){
							if(errno!=EWOULDBLOCK){
								perror("accept() failed");
								end_server=TRUE;
							}
							break;
						}
						//Einkommende Verbindungen zum Master_set hinzufuegen
						printf("New incoming connection - %d\n",new_sd);
						FD_SET(new_sd,&master_set);
						if(new_sd>max_sd)max_sd=new_sd;
						//Naechste Verbindung akzeptieren
					}while(new_sd!=-1);
				}else{//Das ist nicht der Hauptsocket, andere empfangende Verbindung
					printf("Descriptor %d is readable\n",i);
					close_conn=FALSE;
					//Alle Daten abholen
					do{
						//Daten abholen bis Ende (EWOULDBLOCK), sonstige Fehler==>Ende
						rc=recv(i,buffer,sizeof(buffer),0);
						if(rc<0){
							if(errno!=EWOULDBLOCK){
								perror("recv() failed");
								close_conn=TRUE;
							}
							break;
						}
						//Wurde die Verbindung vom Client getrennt
						if(rc==0){
							printf("Connection closed\n");
							close_conn=TRUE;
							break;
						}
						//Alle Daten erhalten
						len=rc;
						printf("%d bytes received\n",len);
						//Echo der Daten zurueck an den Client
						rc=send(i,buffer,len,0);
						if(rc<0){
							perror("send() failed");
							close_conn=TRUE;
							break;
						}
					}while(TRUE);
					//Flag close_conn gesetzt==>Aktive Verbindungen aufraeumen
					//==>Deskriptor vom Master_set entfernen und neues Maximum auf Basis der gesetzten Bits bestimmen
					if(close_conn){
						close(i);
						FD_CLR(i,&master_set);
						if(i==max_sd){
							while(FD_ISSET(max_sd,&master_set)==FALSE)max_sd--;
						}
					}
				}//Ende von existierenden empfangenden Verbindungen
			}//Ende von if(FD_ISSET(i,&working_set))
		}//Ende des Schleife for(i=0;(i<=max_sd)&&(desc_ready>0);++i) ueber vorhandene Deskriptoren
	}while(end_server==FALSE);
	//Aufraeumen
	for(i=0;i<=max_sd;++i){
		if(FD_ISSET(i,&master_set))close(i);
	}
}
