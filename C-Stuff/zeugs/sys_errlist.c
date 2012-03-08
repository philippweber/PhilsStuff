#include <stdio.h>
int main(int argc,char*argv[]){
	int i;
	//for(i=0;i<sys_nerr;i++)puts(sys_errlist[i]);
	printf("%d\n",sys_nerr);
	for(i=0;i<sys_nerr;i++)puts(strerror(i));
	puts("Finito");
	return 0;
}
