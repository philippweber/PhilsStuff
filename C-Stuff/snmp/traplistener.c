/*****
 * Bei STRG-C das Signal abfangen und den Filedescriptor zumachen mit CLOSE!!!
 * Was ist mit IPv6???
 * SNMPv1 vs SNMPv2
 * Sollte ich FORK benutzen
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
//#include <asn1type.h>

struct oidstruct {
	unsigned char *oidtext;
	unsigned char *value;
	unsigned int oidsize;
	unsigned int valuesize;
	unsigned char valuetype;
};

struct oidstructlist {
	struct oidstruct oid;
	struct oidstructlist *next;
};

const int packetsize=1500;

/*****
 * Dekodiert OIDs in ein Human-Readable-Format
 * Maximum von 4 Bytes zwischen 2 Punkten :)
 */
unsigned int oid2HR(unsigned char *trapoid, struct oidstructlist *oid){
	unsigned int newoidsize,charoidsize,pos;//Groesse des Textes in HRFormat, im Trap als BER/ASN.1-Format, Position
	char copypos;//Position fuer Kopie (max. 10, s.u.)
	unsigned long newnumber;//Sollte reichen, Pech wenns groesser is...
	unsigned char *oidtext, oidnum[10]; //Unsigned long <= 10 Stellen (Dez.)
	charoidsize=oid->oid.oidsize;
	//Groesse der Uebersetzung bestimmen...
	//Schaetzung: 1 Byte fuer '.', 3 Byte pro Zahl (1 Byte), d.h. 4*Byte
	//Fuer erste Stelle gilt: Sollte 43 sein, wird zu 1.3. (4 Byte)
	//An 2. Stelle eine 6 (Departement of Defense)
	//An 3. Stelle eine 1 (Internet), am Ende eine '\0'
	//Macht 4*(Groesse-3)+4+2+2+1=4*(Groesse-1)+1 => 4*Groesse
	oidtext=malloc(sizeof(unsigned char)*(charoidsize<<2));//sizeof(unsigned char)==1
	//Uebersetzen
	if(trapoid[0]!=43)return-1;
	if(trapoid[1]!=6)return-1;
	if(trapoid[2]!=1)return-1;
	oidtext[0]='1';oidtext[1]='.';
	oidtext[2]='3';oidtext[3]='.';
	oidtext[4]='6';oidtext[5]='.';
	oidtext[6]='1';
	newoidsize=7;
	newnumber=0;
	for(pos=3;pos<charoidsize;pos++){
		newnumber<<=7;//Eigentlich unwichtig bei newnumber==0, Vergleich lohnt sich aber ned...
		if(trapoid[pos]&0x80){
			newnumber+=trapoid[pos]&0x7F;
		}else{
			oidtext[newoidsize]='.';
			newoidsize++;
			newnumber+=trapoid[pos];//7 Bits uebertragen...
			for(copypos=0;newnumber!=0;copypos++){
				oidnum[(unsigned int)copypos]=(newnumber%10)+'0';
				newnumber/=10;
			}
			for(;copypos>0;newoidsize++){
				copypos--;
				oidtext[newoidsize]=oidnum[(unsigned int)copypos];
			}
			//for(;(newnumber>7)&&(oidtext[newnumber]!='.');newnumber++)
			newnumber=0;
		}
	}
	oidtext[newoidsize]='\0';//Mit calloc unnoetig
	oid->oid.oidsize=newoidsize;
	oid->oid.oidtext=oidtext;
	return charoidsize;
}

/*****
 * Befreit die OIDListe von ihren Schmerzen
 */
void freeOIDList(struct oidstructlist *oidlisthead){
	struct oidstructlist *oidlist,*oidlistnext;
	oidlist=oidlisthead;
	for(oidlistnext=oidlist->next;oidlistnext!=NULL;oidlistnext=oidlist->next){
		free(oidlist->oid.oidtext);
		free(oidlist->oid.value);
		free(oidlist);
		oidlist=oidlistnext;
	}
	free(oidlist->oid.oidtext);
	free(oidlist->oid.value);
	free(oidlist);
}

/*****
 * Analysiert SNMP-Inhalte in
 * Basic Encoding Rules (BER)
 * von SNMP-TRAPS
 */
void TrapAnalyze(unsigned char *trap){
	unsigned int maxsize,pos,version,comnamesize,pdusize;//Maximale Groesse, Position, Version, Kommunennamengroesse, PDU-Groesse
	unsigned int oidlistsize,pdudatasize,pdudatavarbindsize;//Groesse der OID-Liste, PDU-Datengroesse, Groesse der variablen Bindungen der PDU-Daten
	unsigned char addresse[4], gentrapnumber, spectrapnumber;//Adresse mit INET_ADDRSTRLEN==4, generelle/spezifische Trapnummer
	unsigned long timetick;//Zeitstempel
	char *comname;//Kommunenname
	struct oidstructlist *oidlist,*oidlistlast;//Oidliste, Listenende
	if(trap[0]!=0x30){
		perror("No SNMP Package (0x30)");
		return;
	}
	//Laenge
	if(trap[1]&0x80){
		//Langform
		switch(trap[1]&0x7F){
			case 1 : maxsize=(trap[2]&0xFF)+3; pos=3; break;
			case 2 : maxsize=(((unsigned int)(trap[3]))&0xFFFF)+4; pos=4; break;
			default: perror("Size too big (Maxsize)"); return; break;
		}
	}else{
		//Kurzform
		maxsize=trap[1]+2;//&0x7F aber MSB==0
		pos=2;
	}
	//Stimmt die Packetgroesse?
	if(maxsize>packetsize){
		perror("Size too big (Maxsize)");
		return;
	}
	//Mindestgroesse == Momentane Position + Version (3 Byte) + Kommunenname (mind. 3 Byte) + PDU (mind. 3 Byte)
	if((pos+9)>maxsize){
		perror("Size too small (Maxsize)");
		return;
	}
	//Version
	if(trap[pos]!=0x02){
		perror("No SNMP Package (Version)");
		return;
	}
	pos++;
	if(trap[pos]!=0x01){
		perror("No SNMP Package (Versionlength)");
		return;
	}
	pos++;
	version=trap[pos];
	pos++;
	//Kommunenname
	if(trap[pos]!=0x04){
		perror("No SNMP Package (Community)");
		return;
	}
	pos++;
	if(trap[pos]&0x80){
		//Langform
		switch(trap[pos]&0x7F){
			case 1 : pos++; comnamesize=trap[pos]&0xFF; pos++; break;
			case 2 : pos+=2; comnamesize=((unsigned int)(trap[pos]))&0xFFFF; pos++; break;
			default: perror("Size too big (Community)"); return; break;
		}
	}else{
		//Kurzform
		comnamesize=trap[pos];//&0x7F aber MSB==0
		pos++;
	}
	//Stimmt die Stringgroesse?
	if(comnamesize>maxsize){
		perror("Size too big (Comnamesize)");
		return;
	}
	comname=malloc((comnamesize+1)*sizeof(unsigned char));//sizeof(unsigned char)==1, +1 fuer '\0'
	if(comname==NULL){
		perror("No Memory available (Community)");
		return;
	}
	strncpy(comname,(char *)trap+pos,comnamesize);
	comname[comnamesize]='\0';
	pos+=comnamesize;
	//PDU
	if(((trap[pos]!=0xA7)||(version!=1))&&((trap[pos]!=0xA4)||(version!=0))){
		//Keine Trap oder die Version stimmt nicht ueberein
		perror("Keine Trap-PDU");
		free(comname);
		return;
	}
	pos++;
	if(trap[pos]&0x80){
		//Langform
		switch(trap[pos]&0x7F){
			case 1 : pos++; pdusize=trap[pos]&0xFF; pos++; break;
			case 2 : pos+=2; pdusize=((unsigned int)(trap[pos]))&0xFFFF; pos++; break;
			default: perror("Size too big (PDU)"); free(comname); return; break;
		}
	}else{
		//Kurzform
		pdusize=trap[pos];//&0x7F aber MSB==0
		pos++;
	}
	//Stimmt die PDUgroesse?
	if(pdusize>maxsize){
		perror("Size too big (PDUsize)");
		free(comname);
		return;
	}
	printf("SNMP-Trap der Groesse %u, Version %u, Kommunenname %s, PDUgroesse %u\n",maxsize,version+1,comname,pdusize);
	free(comname);
	//HIER NUR SNMPv1
	//PDU-Header-Analyse
	//Enterprise OID
	if(trap[pos]!=6){
		perror("Keine Enterprise OID");
		return;
	}
	pos++;
	oidlistsize=1;
	oidlist=calloc(1,sizeof(struct oidstructlist));
	oidlistlast=oidlist;//Letzter=Erster
	if(oidlist==NULL){
		perror("No Memory available (Oidlist)");
		return;
	}
	if(trap[pos]&0x80){
		//Langform
		switch(trap[pos]&0x7F){
			case 1 : pos++; oidlist->oid.oidsize=trap[pos]&0xFF; pos++; break;
			case 2 : pos+=2; oidlist->oid.oidsize=((unsigned int)(trap[pos]))&0xFFFF; pos++; break;
			default: perror("Size too big (Oid)"); free(oidlist); return; break;
		}
	}else{
		//Kurzform
		oidlist->oid.oidsize=trap[pos];//&0x7F aber MSB==0
		pos++;
	}
	//Stimmt die OIDgroesse?
	if((oidlist->oid.oidsize>pdusize)){
		perror("Size too big (OIDsize)");
		return;
	}
	pos+=oid2HR(trap+pos,oidlist);
	//IP-Addresse
	if(trap[pos]!=0x40){
		perror("Keine IP-Adresse");
		free(oidlist->oid.oidtext);
		free(oidlist);
		return;
	}
	pos++;
	if(trap[pos]!=4){
		perror("Size too big (IPv4)");
		free(oidlist->oid.oidtext);
		free(oidlist);
		return;
	}
	pos++;
	addresse[0]=trap[pos];pos++;
	addresse[1]=trap[pos];pos++;
	addresse[2]=trap[pos];pos++;
	addresse[3]=trap[pos];pos++;
	//Generische Trapnummer
	if(trap[pos]!=2){
		perror("Kein Integer");
		free(oidlist->oid.oidtext);
		free(oidlist);
		return;
	}
	pos++;
	if(trap[pos]!=1){
		perror("Size too big (Int)");
		free(oidlist->oid.oidtext);
		free(oidlist);
		return;
	}
	pos++;
	gentrapnumber=trap[pos];pos++;
	//Spezifische Trapnummer
	if(trap[pos]!=2){
		perror("Kein Integer");
		free(oidlist->oid.oidtext);
		free(oidlist);
		return;
	}
	pos++;
	if(trap[pos]!=1){
		perror("Size too big (Int)");
		free(oidlist->oid.oidtext);
		free(oidlist);
		return;
	}
	pos++;
	spectrapnumber=trap[pos];pos++;
	//In den Enterprise-Oid als Wert...
	oidlist->oid.valuesize=1;
	oidlist->oid.value=malloc(sizeof(unsigned char));//==1
	oidlist->oid.value[0]=spectrapnumber;
	oidlist->oid.valuetype=1;
	//Zeitstempel
	if(trap[pos]!=0x43){
		perror("Kein Timetick");
		free(oidlist->oid.value);
		free(oidlist->oid.oidtext);
		free(oidlist);
		return;
	}
	pos++;
	timetick=0;
	switch(trap[pos]){
		case 1: pos++; ((unsigned char *)(&timetick))[0]=(trap[pos])&0xFF; break;
		case 2: pos++; ((unsigned char *)(&timetick))[1]=(trap[pos])&0xFF;
				pos++; ((unsigned char *)(&timetick))[0]=(trap[pos])&0xFF; break;//timetick=(((unsigned long *)trap)[pos])&0xFFFF;
		case 3: pos++; ((unsigned char *)(&timetick))[2]=(trap[pos])&0xFF;
				pos++; ((unsigned char *)(&timetick))[1]=(trap[pos])&0xFF;
				pos++; ((unsigned char *)(&timetick))[0]=(trap[pos])&0xFF; break;//timetick=(((unsigned long *)trap)[pos])&0xFFFFFF;
		case 4: pos++; ((unsigned char *)(&timetick))[3]=(trap[pos])&0xFF;
				pos++; ((unsigned char *)(&timetick))[2]=(trap[pos])&0xFF;
				pos++; ((unsigned char *)(&timetick))[1]=(trap[pos])&0xFF;
				pos++; ((unsigned char *)(&timetick))[0]=(trap[pos])&0xFF; break;//timetick=((unsigned long *)trap)[pos];
		default:perror("Size too big (Timetick)");
				free(oidlist->oid.value);
				free(oidlist->oid.oidtext);
				free(oidlist);
				return;
			break;
	}
	pos++;
	printf("PDU-Header besagt:\nEnterprise-OID: %s\nIp-Adresse: %hu.%hu.%hu.%hu\n",oidlist->oid.oidtext,addresse[0],addresse[1],addresse[2],addresse[3]);
	printf("Trapnummern (generell/spezifisch): %hu / %hu; bedeutet: ",gentrapnumber,spectrapnumber);
	switch(gentrapnumber){
		case 0: printf("Kaltstart"); break;
		case 1: printf("Warmstart"); break;
		case 2: printf("LinkDown"); break;
		case 3: printf("LinkUp"); break;
		case 4: printf("Authentifikationfehler"); break;
		case 5: printf("Nachbarn verloren"); break;
		case 6: printf("Unternehmensspezifisch, daher OID %s.0.%u",oidlist->oid.oidtext,spectrapnumber); break;
		default: perror("Ungueltige generelle Trapnummer");break;
	}
	putchar('\n');
	printf("Zeitstempel: %lu (Tage: %lu, Stunden: %lu, Minuten: %lu, Sekunden:%lu, Hundertstelsek.: %lu)\n",timetick,timetick/8640000,(timetick/360000)%24,(timetick/6000)%60,(timetick/100)%60,timetick%100);
	//PDU-Data
	if((trap[pos]!=0x30)){
		perror("Kein Sequenzkonstruktor (PDU-Data)");
		free(oidlist->oid.value);
		free(oidlist->oid.oidtext);
		free(oidlist);
		return;
	}
	pos++;
	if(trap[pos]&0x80){
		//Langform
		switch(trap[pos]&0x7F){
			case 1 : pos++; pdudatasize=trap[pos]&0xFF; pos++; break;
			case 2 : pos+=2; pdudatasize=((unsigned int)(trap[pos]))&0xFFFF; pos++; break;
			default: perror("Size too big (PDUData)");
					free(oidlist->oid.value);
					free(oidlist->oid.oidtext);
					free(oidlist); return;
				break;
		}
	}else{
		//Kurzform
		pdudatasize=trap[pos];//&0x7F aber MSB==0
		pos++;
	}
	//Stimmt die PDU-Datengroesse?
	if(pdudatasize>pdusize){
		perror("Size too big (PDUDataSize)");
		free(oidlist->oid.value);
		free(oidlist->oid.oidtext);
		free(oidlist);
		return;
	}
	printf("PDU-Datengroesse: %u\n",pdudatasize);
	while(pos<maxsize){
		if((trap[pos]!=0x30)){
			perror("Kein Sequenzkonstruktor (Variable Bindung)");
			freeOIDList(oidlist);
			return;
		}
		pos++;
		if(trap[pos]&0x80){
			//Langform
			switch(trap[pos]&0x7F){
				case 1 : pos++; pdudatavarbindsize=trap[pos]&0xFF; pos++; break;
				case 2 : pos+=2; pdudatavarbindsize=((unsigned int)(trap[pos]))&0xFFFF; pos++; break;
				default: perror("Size too big (Variable Bindung)");
						freeOIDList(oidlist);
						return;
					break;
			}
		}else{
			//Kurzform
			pdudatavarbindsize=trap[pos];//&0x7F aber MSB==0
			pos++;
		}
		//Stimmt die Groesse der variablen Bindungen?
		if(pdudatavarbindsize>pdudatasize){
			perror("Size too big (PDUDataVarBindSize)");
			freeOIDList(oidlist);
			return;
		}
		//Objektidentifikator
		if(trap[pos]!=6){
			perror("Kein Objektidentifikator");
			freeOIDList(oidlist);
			return;
		}
		pos++;
		oidlistsize++;
		oidlistlast->next=calloc(1,sizeof(struct oidstructlist));
		oidlistlast=oidlistlast->next;
		if(oidlistlast==NULL){
			perror("No Memory available (Oidlist)");
			freeOIDList(oidlist);
			return;
		}
		if(trap[pos]&0x80){
			//Langform
			switch(trap[pos]&0x7F){
				case 1 : pos++; oidlistlast->oid.oidsize=trap[pos]&0xFF; pos++; break;
				case 2 : pos+=2; oidlistlast->oid.oidsize=((unsigned int)(trap[pos]))&0xFFFF; pos++; break;
				default: perror("Size too big (Oid)");
						freeOIDList(oidlist);
						return;
					break;
			}
		}else{
			//Kurzform
			oidlistlast->oid.oidsize=trap[pos];//&0x7F aber MSB==0
			pos++;
		}
		//Stimmt die OIDgroesse?
		if((oidlistlast->oid.oidsize>pdudatavarbindsize)){
			perror("Size too big (OIDsize)");
			freeOIDList(oidlist);
			return;
		}
		pos+=oid2HR(trap+pos,oidlistlast);
		//Wert
		oidlistlast->oid.valuetype=trap[pos];
		pos++;
		if(trap[pos]&0x80){
			//Langform
			switch(trap[pos]&0x7F){
				case 1 : pos++; oidlistlast->oid.valuesize=trap[pos]&0xFF; pos++; break;
				case 2 : pos+=2; oidlistlast->oid.valuesize=((unsigned int)(trap[pos]))&0xFFFF; pos++; break;
				default: perror("Size too big (Value)");
						freeOIDList(oidlist);
						return;
					break;
			}
		}else{
			//Kurzform
			oidlistlast->oid.valuesize=trap[pos];//&0x7F aber MSB==0
			pos++;
		}
		//Stimmt die Wertgroesse?
		if((oidlistlast->oid.valuesize>pdudatavarbindsize)){
			perror("Size too big (ValueSize)");
			freeOIDList(oidlist);
			return;
		}
		oidlistlast->oid.value=malloc(sizeof(unsigned char)*oidlistlast->oid.valuesize);
		memcpy(oidlistlast->oid.value,trap+pos,oidlistlast->oid.valuesize);
		pos+=oidlistlast->oid.valuesize;
		printf("OIDgroesse: %u OID: %s Wertgroesse: %u Werttyp: %hu\n",oidlistlast->oid.oidsize,oidlistlast->oid.oidtext,oidlistlast->oid.valuesize,oidlistlast->oid.valuetype);
	}
	printf("Komplette OIDListe (Groesse %u):\n",oidlistsize);
	for(oidlistlast=oidlist;oidlistlast!=NULL;oidlistlast=oidlistlast->next){
		printf("OID: %s\n  Werttyp: %hu\n   Dump:\n    ",oidlistlast->oid.oidtext,oidlistlast->oid.valuetype);
		for(pos=0;pos<oidlistlast->oid.valuesize;pos++){
			printf(" %3hX",oidlistlast->oid.value[pos]);
		}
		printf("\n    ");
		for(pos=0;pos<oidlistlast->oid.valuesize;pos++){
			printf(" %3hu",oidlistlast->oid.value[pos]);
		}
		printf("\n    ");
		for(pos=0;pos<oidlistlast->oid.valuesize;pos++){
			if((oidlistlast->oid.value[pos]>31)&&(oidlistlast->oid.value[pos]<127))
					printf(" %3c",oidlistlast->oid.value[pos]);
				else
					printf(" ???");
		}
		putchar('\n');
	}
	putchar('\n');
	freeOIDList(oidlist);
}


/*****
 * Oeffnet den Server an dem Port
 */
void * UDPSock (int PortNr) {
	int server_socky;//Server-Socket
	socklen_t address_length;//Addresslaenge (unsigned int)
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
	printf ("UDP-Server wartet...\n");
	while (1) {
		//Empfange...
		address_length = sizeof (client_address);
		if (recvfrom (server_socky, trap, packetsize, 0, (struct sockaddr *) &client_address, &address_length) == -1) {
		//if (recvfrom (server_socky, &trapsize, sizeof(int), 0, NULL, 0) == -1) {
			perror ("Read");
		}
		inet_ntop(AF_INET,(struct in_addr *) &client_address.sin_addr,addresse,INET_ADDRSTRLEN);
		printf("Client:\nFamily: %hd\nPort: %hu\nAddresse: %lu\nAddresse: %s\n",client_address.sin_family,client_address.sin_port,(unsigned long)client_address.sin_addr.s_addr,addresse);
		TrapAnalyze(trap);
		putchar('\n');
		/*printf("Hex-Dump:\n");
		int i;
		for(i=0;i<packetsize;i++)printf("%hX ",((unsigned char)trap[i]&0xFF));
		putchar('\n');*/
	}
}


int main (int argc, char *argv[]) {
	//UDPSock (162);	//162 durch KONFIGURATIONSDATEI festlegen!!
	UDPSock (65533);	//162 durch KONFIGURATIONSDATEI festlegen!!
	exit (0);
}
