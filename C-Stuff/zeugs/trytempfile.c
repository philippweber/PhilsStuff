#include<stdio.h>
#include<unistd.h>
#include<signal.h>
#include<string.h>
void signalFunction(int sig){}
int main(int argc,char*argv[]){
	signal(SIGUSR1,signalFunction);
	signal(SIGUSR2,signalFunction);
	printf("%d\n",getpid());
	printf("ll /proc/%d/fd/\n",getpid());
	printf("kill -s SIGUSR1 %d\n",getpid());
	char tempfilename[L_tmpnam];
	tmpnam(tempfilename);
	printf("%s\n",tempfilename);
	puts(tempnam(NULL,"test"));//Fehlt free!
	//FILE*tempfile=tmpfile();
	puts(P_tmpdir);
	char testfilename[27];
	memcpy(testfilename,"/tmp/testestestXXXXXXXXXX",26);
	testfilename[26]='\000';
	int testfd=mkstemp(testfilename);
	printf("%d\n%s\n",testfd,testfilename);
	FILE*tempfile=fdopen(testfd,"w+");
	fprintf(tempfile,"Hallo Welt\n");
	perror("fprintf");
	pause();
	char testtext[1000];
	rewind(tempfile);
	fscanf(tempfile,"%s",testtext);
	perror("fscanf");
	puts(testtext);
	fclose(tempfile);
	pause();
}

