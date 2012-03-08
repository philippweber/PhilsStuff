/*****
 * Dekodiert ein BER-codiertes Packet
 * (Speziell fuer SNMP)
 * Dekodiert die Typen...
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
//#include <asn1type.h>

const int packetsize=1500;

/*****
 * Erzeugt aus einem Hex-String einen Time-Tick (max. 4 Byte)
 */
void char2tt(unsigned char *token, unsigned int datasize){
	unsigned long timetick=0;
	unsigned char newtoken[4]={0,0,0,0};
	printf("%lx\n%02hX %02hX %02hX %02hX\n",*((unsigned long *)(newtoken)),newtoken[0],newtoken[1],newtoken[2],newtoken[3]);
	//Schneller als FOR :)
	switch(datasize){
		case 0: printf("Empty"); return; break;
		case 1: newtoken[3]=token[0];break;
		case 2: newtoken[3]=token[1];newtoken[2]=token[0];break;
		case 3: newtoken[3]=token[2];newtoken[2]=token[1];newtoken[1]=token[0];break;
		case 4: newtoken[3]=token[3];newtoken[2]=token[2];newtoken[1]=token[1];newtoken[0]=token[0];break;
		default: printf("Size too big"); return; break;
	}
	timetick=ntohl(*((unsigned long *)newtoken));
	printf("%lX\n%02hX %02hX %02hX %02hX\n",*((unsigned long *)(newtoken)),newtoken[0],newtoken[1],newtoken[2],newtoken[3]);
	printf("%lX\n",timetick);
	printf("Zeitstempel: %lu (Tage: %lu, Stunden: %lu, Minuten: %lu, Sekunden:%lu, Hundertstelsek.: %lu)",timetick,timetick/8640000,(timetick/360000)%24,(timetick/6000)%60,(timetick/100)%60,timetick%100);
}

/*****
 * Erzeugt aus einem Hex-String einen Dec-String (max. 4 Byte)
 */
void char2int(unsigned char *token, unsigned int datasize){
	long newnumber=0;
	unsigned char newtoken[4]={0,0,0,0};
	switch(datasize){
		case 0: printf("Empty"); return; break;
		case 1: newtoken[3]=token[0];break;
		case 2: newtoken[3]=token[1];newtoken[2]=token[0];break;
		case 3: newtoken[3]=token[2];newtoken[2]=token[1];newtoken[1]=token[0];break;
		case 4: newtoken[3]=token[3];newtoken[2]=token[2];newtoken[1]=token[1];newtoken[0]=token[0];break;
		default: printf("Size too big"); return; break;
	}
	newnumber=ntohl(*((unsigned long *)newtoken));
	printf("%ld",newnumber);
}

/*****
 * Dekodiert OIDs in ein Human-Readable-Format
 * Maximum von 4 Bytes zwischen 2 Punkten :)
 */
void oid2HR(unsigned char *trapoid, unsigned int datasize){//Groesse im Trap als BER/ASN.1-Format
	unsigned int pos, oldpos;//Position, Position letzter Punkt
	unsigned long newnumber;//Sollte reichen, Pech wenns groesser is => {Size too big}, s.u.
	//Uebersetzen
	printf("%u.%u",trapoid[0]/40,trapoid[0]%40);
	newnumber=0;
	oldpos=0;
	for(pos=1;pos<datasize;pos++){
		newnumber<<=7;
		newnumber+=trapoid[pos]&0x7F;
		if(trapoid[pos]&0x80){
			if(pos-oldpos>3){
				printf(".{Size too big:%lX}",newnumber);
				newnumber=0;
			}
		}else{
			printf(".%lu",newnumber);
			newnumber=0;
			oldpos=pos;
		}
	}
}

/*****
 * Basic Encoding Rules (BER)
 * in HTML verwandeln...
 */
unsigned int BER2HTML(unsigned char *token, unsigned int pos, unsigned int max){
	//max=0 unbestimmtes Format, 2mal EoC (Ende des Inhalts erwartet...)
	char constructed,vartype;//vartype: Bitvektor: TimeTicks|IP|Oid|Truedec|String|Dec|Hex 
	unsigned int newpos=pos,newmax,datasize;
	printf("<table style=\"border:1px solid green;\"><tr><td align=\"left\" valign=\"top\" style=\"border:1px solid red;\">Typ</td><td align=\"left\" valign=\"top\" style=\"border:1px solid red;\">Laenge</td><td align=\"left\" valign=\"top\" style=\"border:1px solid red;\">Variable</td></tr>");
	while(((max==0)||(newpos<max))&&(newpos<packetsize)){
		//Analysiere die Token einer Ebene...
		printf("<tr><td align=\"left\" valign=\"top\" style=\"border:1px solid red;\">Position %u : %02hX<br />Klasse: ",newpos,((unsigned char)token[newpos]&0xFF));
		vartype=7;//Hex+Dec+String, Standard
		switch((token[newpos]>>6)&3){
			case 0 : printf("Universell<br />");
				printf("Tag: ");
				switch(token[newpos]&0x1F){
					//Die Uebersetzungen sind teilweise etwas Scheisse...
					case 0 : if(max==0){
							//max=0 unbestimmtes Format, 2mal EoC (Ende des Inhalts erwartet...)
							printf("Ende des Inhalts<br /></td><td /><td /></tr>");
							newpos++;
							if(token[newpos]!=0)return packetsize;//Abbruch
							printf("<tr><td align=\"left\" valign=\"top\" style=\"border:1px solid red;\">Position %u : %02hX<br />Klasse: Universell<br />Tag: Ende des Inhalts<br /></td><td /><td /></tr></table>",newpos,((unsigned char)token[newpos]&0xFF));
							return newpos;
						}else{
							printf("Ende des Inhalts<br></td><td /><td /></tr>");
							newpos++;
							continue;
						}
						break;
					case 1 : printf("Boolean"); vartype=1; break;
					case 2 : printf("Integer"); vartype=9; break;
					case 3 : printf("Bitstring"); vartype=5; break;
					case 4 : printf("Octetstring"); vartype=5; break;
					case 5 : printf("NULL"); break;
					case 6 : printf("Objektidentifikator"); vartype=0x11; break;
					case 7 : printf("Objektdeskriptor"); break;
					case 8 : printf("Extern"); break;
					case 9 : printf("Float"); break;
					case 10: printf("Nummerierung"); break;
					case 11: printf("Eingebettetes PDV"); break;
					case 12: printf("UTF8String"); vartype=5; break;
					case 13: printf("Relative OID"); vartype=0x11; break;
					case 16: printf("Sequenzen"); break;
					case 17: printf("Mengen"); break;
					case 18: printf("Nummerischer String"); break;
					case 19: printf("Druckbarer String"); vartype=5; break;
					case 20: printf("T61String"); break;
					case 21: printf("VideotexString"); break;
					case 22: printf("IA5String"); break;
					case 23: printf("UTCTime"); break;
					case 24: printf("Generalisierte Zeit"); break;
					case 25: printf("Graphischer String"); break;
					case 26: printf("Sichtbarer String"); break;
					case 27: printf("Genereller String"); break;
					case 28: printf("Universeller String"); break;
					case 29: printf("Charakter String"); vartype=5; break;
					case 30: printf("BMPString"); break;
					case 31: printf("Fortsetzung &nbsp;");//Sollte nicht vorkommen, da alles in
						int i=1;
						//Wert des Tags steht in Bits 0-6, MSB=1 bis abbruch...
						for(;(token[newpos+i]&0x80)&&(i+newpos<packetsize);i++)printf("%d. Tag: %02hX &nbsp;",i,token[newpos+i]&0x7F);
						break;
					default: printf("Unbekannt (%02hX)",token[newpos]&0x1F); break;
				}
				break;
			case 1 : printf("Anwendung<br />");
				//Tag
				printf("Tag: ");
				//Siehe RFC1155 3.2.3 (SNMPv1), (SNMPv2) RFC2578 7.1 und RFC3416 2.5/3
				switch(token[newpos]&0x1F){
					case 0 : printf("IpAddresse"); vartype=0x22; break;
					case 1 : printf("Counter"); vartype=9; break;
					case 2 : printf("Gauge"); vartype=9; break;
					case 3 : printf("TimeTicks"); vartype=0x41; break;
					case 4 : printf("Opaque"); break;
					case 5 : printf("NsapAddress"); break;
					case 6 : printf("Counter64"); vartype=9; break;
					case 7 : printf("Uinteger32"); vartype=9; break;
					default: printf("Unbekannt (%02hX)",token[newpos]&0x1F); break;
				}
				break;
			case 2 : printf("Context-spezifisch<br />");
				//Tag
				printf("Tag: ");
				//Siehe RFC1155 3.2.3 (SNMPv1), (SNMPv2) RFC2578 7.1 und RFC3416 2.5/3
				switch(token[newpos]&0x1F){
					case 0 : printf("GetRequest-PDU"); break;
					case 1 : printf("GetNextRequest-PUD"); break;
					case 2 : printf("GetResponse-PDU"); break;
					case 3 : printf("SetRequest-PDU"); break;
					case 4 : printf("Trap-PDU"); break;
					case 5 : printf("GetBulkRequest-PDU"); break;
					case 6 : printf("InformRequest-PDU"); break;
					case 7 : printf("SNMPv2-Trap-PDU"); break;
					default: printf("Unbekannt (%02hX)",token[newpos]&0x1F); break;
				}
				break;
			break;
			case 3 : printf("Privat<br />Tag: %02hX",token[newpos]&0x1F); break;
		}
		//Typ
		printf("<br />Typ: ");
		constructed=token[newpos]&0x20;
		if(constructed)printf("Konstruiert<br />");
			else printf("Primitiv<br />");
		//Laenge
		newpos++;
		printf("</td><td align=\"left\" valign=\"top\" style=\"border:1px solid red;\">Position %u : %02hX<br />",newpos,((unsigned char)token[newpos]&0xFF));
		if((max!=0)&&(newpos>=max))return packetsize;//Abbruch
		if(token[newpos]==0x80){
			datasize=0;
			printf("Unbestimmte Laenge\n");
			//return packetsize;//Abbruch, Groesse unbekannt...
		}else if(token[newpos]&0x80){
			//Langform
			printf("Langform der Laenge: %d<br />",token[newpos]&0x7F);
			datasize=0;
			switch(token[newpos]&0x7F){
				case 1 :newpos++; datasize=token[newpos]&0xFF; newpos++; break;
				case 2 :newpos++; datasize=ntohs(*((unsigned int *)(token+newpos))); newpos+=2; break;
				default:printf("Variable zu gross\n"); return packetsize; break;
			}
		}else{
			//Kurzform
			printf("Kurzform<br />");
			datasize=token[newpos];//&0x7F aber MSB==0
			newpos++;
		}
		printf("Variabellaenge %u</td><td align=\"left\" valign=\"top\" style=\"border:1px solid red;\">",datasize);
		if(constructed){
			if(datasize==0)newmax=0;else newmax=datasize+newpos;
			newpos=BER2HTML(token,newpos,newmax);
		}else if(datasize>0){
			//Daten
			printf("Position von %u bis %u :",newpos,newpos+datasize-1);
			if(vartype&1){//Hexausgabe
				printf("<pre>");
				newmax=newpos;//Missbrauch als Counter
				for(;(newmax<newpos+datasize)&&(newmax<max);newmax++){
					printf("%02hX ",((unsigned char)token[newmax]&0xFF));
				}
				printf("</pre>");
			}
			if(vartype&2){//Dezausgabe
				printf("<pre>");
				newmax=newpos;//Missbrauch als Counter
				for(;(newmax<newpos+datasize)&&(newmax<max);newmax++){
					printf("%hu ",((unsigned char)token[newmax]&0xFF));
				}
				printf("</pre>");
			}
			if(vartype&4){//Stringausgabe
				printf("<pre>");
				newmax=newpos;//Missbrauch als Counter
				for(;(newmax<newpos+datasize)&&(newmax<max);newmax++){
					if((((unsigned char)token[newmax]&0xFF)>31)&&(((unsigned char)token[newmax]&0xFF)<127))
							printf("%c",((unsigned char)token[newmax]&0xFF));
						else
							printf(" ??? ");
				}
				printf("</pre>");
			}
			if(vartype&8){//Truedecausgabe
				printf("<pre>");
				char2int(token+newpos,datasize);
				printf("</pre>");
			}
			if(vartype&0x10){//OIDAusgabe
				printf("<pre>");
				oid2HR(token+newpos,datasize);
				printf("</pre>");
			}
			if(vartype&0x20){//IPAusgabe
				printf("<pre>");
				newmax=newpos;//Missbrauch als Counter
				for(;(newmax<newpos+datasize-1)&&(newmax<max-1);newmax++){
					printf("%hu.",((unsigned char)token[newmax]&0xFF));
				}
				printf("%hu ",((unsigned char)token[newmax]&0xFF));
				printf("</pre>");
			}
			if(vartype&0x40){//TimeTicksAusgabe
				printf("<pre>");
				char2tt(token+newpos,datasize);
				printf("</pre>");
			}
			newpos+=datasize;//newmax==datasize||newmax==max :)
		}else printf("Leer");
		printf("</td></tr>");
	}
	printf("</table>");
	return newpos;
}


/*****
 * Gibt eine HEX DUMP in HTML aus
 */
void hexhtmldump(unsigned char*packet, unsigned int size){
	unsigned int pos=0;
	printf("<table style=\"border:1px solid green;\"><tr>");
	for(;pos<size;pos++){
		printf("<td align=\"left\" valign=\"top\" style=\"border:1px solid red;\">%u : %02hX</td>",pos,((unsigned char)packet[pos]&0xFF));
		if((pos%16)==15)printf("</tr><tr>");
	}
	printf("</tr></table>");
}


/*****
 * Oeffnet den Server an dem Port
 */
void UDPSock (int PortNr) {
	int server_socky;//Server-Socket
	unsigned int address_length;//Addresslaenge
	unsigned char trap[packetsize];//Packetbuffer
	char addresse[INET_ADDRSTRLEN];//Adresse
	//trapbuffer;//Trapbuffer fuer Traps
	struct sockaddr_in server_address, client_address;
	//Server einrichten
	if ((server_socky = socket (AF_INET, SOCK_DGRAM, 0)) == -1) {
		perror ("Socket");
		exit (-1);//(EXIT_FAILURE);
	}
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = htonl (INADDR_ANY);	//Hier sollte nur unser SUBNETZ rein!!!
	server_address.sin_port = htons (PortNr);
	address_length = sizeof (server_address);
	if (bind (server_socky, (struct sockaddr *) &server_address, address_length) == -1) {
		perror ("Bind");//Unlink!!
		exit (-1);//(EXIT_FAILURE);
	}
	//Addresse des Empfaengers
	client_address.sin_family = AF_INET;
	client_address.sin_addr.s_addr = htonl (INADDR_ANY);
	client_address.sin_port = htons (PortNr);
	//Ready
	printf("<html><title>BER2HTML</title><body>");
	//Empfange...
	address_length = sizeof (client_address);
	if (recvfrom (server_socky, trap, packetsize, 0, (struct sockaddr *) &client_address, &address_length) == -1) {
		perror ("Read");
	}
	inet_ntop(AF_INET,(struct in_addr *) &client_address.sin_addr,addresse,INET_ADDRSTRLEN);
	printf("<table style=\"border:1px solid green;\"><tr><td align=\"left\" valign=\"top\" style=\"border:1px solid red;\">Client:</td></tr><tr><td align=\"left\" valign=\"top\" style=\"border:1px solid red;\">Family: %hd &nbsp; Port: %hu</td></tr><tr><td align=\"left\" valign=\"top\" style=\"border:1px solid red;\">Addresse: %lu (%s)</td></tr></table>",
			client_address.sin_family,client_address.sin_port,(unsigned long)client_address.sin_addr.s_addr,addresse);
	//HEXDUMP Groesse bestimmen...
	unsigned int sizeme=0;
	if(trap[1]&0x80){
		//Langform
		switch(trap[1]&0x7F){
			case 1 :sizeme=trap[2]&0xFF; sizeme+=3; break;
			case 2 :sizeme=ntohs(*((unsigned int *)(trap+2)))+4;break;
			default:break;
		}
	}else{
		//Kurzform
		sizeme=trap[1]+2;//&0x7F aber MSB==0
	}
	hexhtmldump(trap,sizeme);//nicht mehr packetsize
	printf("<br />Parse %u Bytes<br />",BER2HTML(trap,0,sizeme));//Mindestens 4 :)
	printf("</body></html>");
	close(server_socky);
}


int main (int argc, char *argv[]) {
	int port=162;
	if(argc<2){
		printf("Zu wenig Argumente\n\n");
		return 1;
	}
	port=atoi(argv[1]);
	UDPSock(port);//162 durch KONFIGURATIONSDATEI festlegen!!
	return 0;
}
