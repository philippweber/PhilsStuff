#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
int main(){
	struct in6_addr*ip1=malloc(sizeof(struct in6_addr)),*ip2=malloc(sizeof(struct in6_addr));
	if(inet_pton(AF_INET6,"::ffff:0.0.0.0",ip1))puts("Guuuuddddd");else puts("Nogud");
	int i=0;
	unsigned char*c=(unsigned char*)ip1;
	for(i=0;i<sizeof(struct in6_addr);i++){printf("%hx ",c[i]);}
	puts("");
	*ip2=*ip1;//Dashier mal testen...
	unsigned char*addr=calloc(1,sizeof(unsigned char)*25);//24+1 :)
	inet_ntop(AF_INET6,ip2,addr,sizeof(unsigned char)*25);
	puts(addr);
	return 0;
}
