#include <byteswap.h>
#include <errno.h>
#include <inttypes.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <net/ethernet.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <netinet/ether.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netinet/in.h>
//#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <netpacket/packet.h>
#include <sys/fcntl.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/time.h>

#define max_packet_size 0x10001+sizeof(struct ethhdr) //Mindestens sizeof(struct ethhdr)+sizeof(struct iphdr)+sizeof(struct udphdr)
#define ETH_P_IP_htons 0x0008 //IP==htons(ETH_P_IP)

int main(int argc,char*argv[]){
	int sock,uid;
	unsigned char packet[max_packet_size];
	//struct sockaddr device_address;
	struct sockaddr_ll device_address;
	struct ifreq device_info;
	//Parameter verarbeiten
	if(argc<8){
		printf("Usage: mac-fake <dev> <ziel-mac> <quell-mac> <quell-ipv4> <ziel-ipv4> <quell-udpport> <ziel-udpport> <data (optional)>\n");
		return 1;
	}
	//Root?
	uid=getuid();
	if(uid!=0){
		printf("UID 0 anstelle von '%d' benoetigt.\n",uid);
		return 1;
	}
	//Erstelle einen Socket Deskriptor
	//if((sock=socket(AF_INET,SOCK_PACKET,htons(ETH_P_ALL)))==-1){
	if((sock=socket(AF_PACKET,SOCK_RAW,ETH_P_IP_htons))==-1){
		perror("Socket: ");
		return 1;
	}
	//An Device binden
	//memset(&device_address,0,sizeof(device_address));
	//device_address.sa_family=AF_INET;
	//<dev>==argv[1]
	//Maximale Groesse von as_data ist 14!?! (IFNAMSIZE==16?)
	//strncpy(device_address.sa_data,argv[1],14);
	//strncpy(device_address.sa_data,argv[1],IFNAMSIZ);
	//if(bind(sock,&device_address,sizeof(device_address))==-1){
		//perror("Bind: ");
		//return 1;
	//}
	//Interface-Informationen (Nummer)
	memset(&device_info,0,sizeof(device_info));//==>sizeof(struct ifreq)
	strncpy(device_info.ifr_name,argv[1],IF_NAMESIZE);
	if(ioctl(sock,SIOCGIFINDEX,&device_info)<0){
		perror("Ioctl: ");
		return 1;
	}
	//Binding Interfaces
	memset(&device_address,0,sizeof(device_address));//==>sizeof(struct sockaddr)
	device_address.sll_family=AF_PACKET;//==device_address.sa.sa_family
	device_address.sll_protocol=ETH_P_IP_htons;
	device_address.sll_ifindex=device_info.ifr_ifindex;
	if(bind(sock,(struct sockaddr*)&device_address,sizeof(device_address))<0){
		perror("BNind: ");
		return 1;
	}
	//Paket zusammenbasteln
	//Pointer setzen
	struct ethhdr*ethernet_header=(struct ethhdr*)packet;//Ethernet
	struct iphdr*ip_header=(struct iphdr*)((char*)ethernet_header+sizeof(struct ethhdr));//IPv4
	struct udphdr*udp_header=(struct udphdr*)((char*)ip_header+sizeof(struct iphdr));//UDP
	//Ethernet-Header
	//<ziel-mac>==argv[2]
	//<quell-mac>==argv[3]
	struct ether_addr*eth_address;
	if((eth_address=ether_aton(argv[2]))==NULL){
		fprintf(stderr,"'%s' ist eine ungueltige Ethernet-Adresse\n",argv[2]);
		return 1;
	}
	memcpy(ethernet_header->h_dest,eth_address,ETH_ALEN);
	if((eth_address=ether_aton(argv[3]))==NULL){
		fprintf(stderr,"'%s' ist eine ungueltige Ethernet-Adresse\n",argv[3]);
		return 1;
	}
	memcpy(ethernet_header->h_source,eth_address,ETH_ALEN);
	ethernet_header->h_proto=htons(ETHERTYPE_IP);//0x0080
	//IP-Header
	//<quell-ipv4>==argv[4]
	//<ziel-ipv4>==argv[5]
	//ip_header->tot_len und ip_header->check koennen erst am Schluss angegeben werden
	//Adressen
	struct in_addr ip_address;
	if(inet_pton(AF_INET,argv[4],&ip_address)!=1){
		//Error in conversion
		fprintf(stderr,"'%s' ist eine ungueltige IP-Adresse\n",argv[4]);
		return 1;
	}
	//memcpy(ip_header->saddr,ip_address.s_addr,sizof(uint32_t));
	ip_header->saddr=ip_address.s_addr;
	if(inet_pton(AF_INET,argv[5],&ip_address)!=1){
		//Error in conversion
		fprintf(stderr,"'%s' ist eine ungueltige IP-Adresse\n",argv[5]);
		return 1;
	}
	ip_header->daddr=ip_address.s_addr;
	//Rest
	ip_header->version=IPVERSION;
	ip_header->ihl=5;//==20 Byte
	ip_header->tos=0;
	ip_header->id=0;//htons(0);
	ip_header->frag_off=0x0040;//Dont fragment!
	ip_header->ttl=IPDEFTTL;
	ip_header->protocol=IPPROTO_UDP;
	//Es fehlen: ip_header->tot_len und ip_header->check
	//UDP-Header
	//<quell-udpport>==argv[6]
	//<ziel-udpport>==argv[7]
	//udp_header->len und udp_header->check koennen erst am Schluss angegeben werden
	char*endstr;
	errno=0;
	udp_header->source=htons(strtol(argv[6],&endstr,10));//atoi(argv[6]);
	if((errno!=0)||(*endstr!='\000')){
		fprintf(stderr,"'%s' ist keine gueltige Zahl\n",argv[6]);
	}
	udp_header->dest=htons(strtol(argv[7],&endstr,10));//atoi(argv[7]);
	if((errno!=0)||(*endstr!='\000')){
		fprintf(stderr,"'%s' ist keine gueltige Zahl\n",argv[7]);
	}
	//Es fehlen: udp_header->len und udp_header->check
	//Daten (falls gewollt)
	//<data>==argv[8]
	int datasize;
	if(argc>8){
		datasize=strlen(argv[8]);
		//Groessenvergleich fuer Platzbedarf
		if(max_packet_size<sizeof(struct ethhdr)+sizeof(struct iphdr)+sizeof(struct udphdr)+datasize){
				fprintf(stderr,"Buffer ist zu klein,Nachricht zu gross\n");
				return 1;
		}
		//Kopieren hinter UDP-Header
		memcpy((char*)udp_header+sizeof(struct udphdr),argv[8],datasize);
	}else{
		datasize=0;
	}
	//Paketpointer
	uint8_t*pc=(uint8_t*)udp_header,*pend=(uint8_t*)packet+sizeof(struct ethhdr)+sizeof(struct iphdr)+sizeof(struct udphdr)+datasize;
	//Fuer netcat
	//datasize++;//Fuer netcat
	//*pend=0x0A;//Fuer netcat
	//pend++;//Fuer netcat
	//Groessen setzen
	udp_header->len=htons((uint16_t)(sizeof(struct udphdr)+datasize));
	ip_header->tot_len=htons((uint16_t)(sizeof(struct iphdr)+sizeof(struct udphdr)+datasize));
	//Checksummen berechnen
	//IP-Header
	long checksum=(ntohs(*((uint16_t*)ip_header)))+(ntohs(((uint16_t*)ip_header)[1]))+(ntohs(((uint16_t*)ip_header)[2]))
								+(ntohs(((uint16_t*)ip_header)[3]))+(ntohs(((uint16_t*)ip_header)[4]))+(ntohs(((uint16_t*)ip_header)[5]))
								+(ntohs(((uint16_t*)ip_header)[6]))+(ntohs(((uint16_t*)ip_header)[7]))+(ntohs(((uint16_t*)ip_header)[8]))
								+(ntohs(((uint16_t*)ip_header)[9]));//20 Byte
	ip_header->check=~htons((uint16_t)((checksum&0xFFFF)+((checksum&0xFFFF0000)>>16)));
	//UDP: Pseudoheader von IPv4
	checksum=(((ip_header->saddr&0xFF)<<8)&0xFFFF)+(((ip_header->saddr&0xFF00)>>8)&0xFFFF)
					+(((ip_header->saddr&0xFF0000)>>8)&0xFFFF)+(((ip_header->saddr&0xFF000000)>>24)&0xFFFF)
					+(((ip_header->daddr&0xFF)<<8)&0xFFFF)+(((ip_header->daddr&0xFF00)>>8)&0xFFFF)
					+(((ip_header->daddr&0xFF0000)>>8)&0xFFFF)+(((ip_header->daddr&0xFF000000)>>24)&0xFFFF)
					//(ip_header->saddr&0xFFFF)+(((ip_header->saddr&0xFFFF0000)>>16)&0xFFFF)
					//+(ip_header->daddr&0xFFFF)+(((ip_header->daddr&0xFFFF0000)>>16)&0xFFFF)
					+(ip_header->protocol&0xFF)+ntohs(udp_header->len);//+ntohs(ip_header->tot_len);
	if(((pc-pend)%2)==0){
		printf("Achtung: Haenge eine Null an das Paket!!!!\n");
		*pend='\000';
		pend++;
	}
	for(;pc<pend;pc++){
		checksum+=((*pc)&0xFF)<<8;
		pc++;
		checksum+=(*pc)&0xFF;
	}
	udp_header->check=~htons((uint16_t)((checksum&0xFFFF)+((checksum&0xFFFF0000)>>16)));
	//Dump
	for(pc=packet;pc<pend;pc++){
		printf("%02hX ",((*pc)&0xFF));
	}
	puts("");
	//Abschicken
	//Init schon passiert...
	//memset(&device_address,0,sizeof(device_address));
	//device_address.sa_family=AF_INET;
	//strncpy(device_address.sa_data,argv[1],IFNAMSIZ);
	//Wozu so kompliziert?
	//if((sendto(sock,packet,sizeof(struct ethhdr)+sizeof(struct iphdr)+sizeof(struct udphdr)+datasize,0,&device_address,sizeof(struct sockaddr)))==-1){
		//perror("Sendto: ");
		//return 1;
	//}
	if(send(sock,packet,sizeof(struct ethhdr)+sizeof(struct iphdr)+sizeof(struct udphdr)+datasize,0)<0){
		perror("Send: ");
		return 1;
	}
	//Alles schliessen
	close(sock);
	return 0;
}
