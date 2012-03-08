#include <stdio.h>
int main(int argc, char*argv[]){
	if(argc<4)return 1;
	int n=atoi(argv[3]);
	printf("%s %s %d\n",argv[1],argv[2],memcmp(argv[1],argv[2],n));
	return 0;
}
