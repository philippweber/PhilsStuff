#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>

#include <openssl/md5.h>
#include <openssl/des.h>

int main(int argc,char*argv[])
{
	long i;//Fuer die Ausgabe
	if(argc<5){printf("Passwort, EngineID, Salt und Datei bitte.\n");return 1;}
	//Lese Datei
	FILE*fp;long fsize;
	fp=fopen(argv[4],"r");
	if(fp==NULL){printf("Error: Kann Datei nicht oeffnen.\n");return 1;}
	fseek(fp,0L,SEEK_END);
	fsize=ftell(fp);
	fseek(fp,0L,SEEK_SET);
	unsigned char*fb=malloc(sizeof(unsigned char)*fsize);
	unsigned char*bp=fb;//Bufferpointer
	while(!feof(fp)){
		fscanf(fp,"%x ",bp);bp++;
	}
	fclose(fp);
	fsize=bp-fb;//Missbrauche fsize
	if((fsize%8)!=0){printf("Error: Falsche Groesse.\n");return 1;}
	//printf("%d\n",fsize);
	//Ausgabe, die gleich aussieht wie im Trac...
	//unsigned char*bp2;for(bp2=fb;bp2<bp;bp2++){if((i%16)==0){puts("");i=0;}else printf(" ");i++;printf("%02hx",*bp2);}
	for(i=0;i<fsize;i++){if((i%16)==0)puts("");else printf(" ");printf("%02hx",fb[i]);}
	//Erstelle den Schluessel
	unsigned char*keyspace=malloc(sizeof(unsigned char)*0x100000);
	//Missbrauche bp
	unsigned char*key=argv[1];
	for(bp=keyspace;bp<keyspace+0x100000;bp++){
		if(*key=='\0')key=argv[1];
		*bp=*key;
		key++;
	}
	//Ausgabe
	//for(i=0;i<0x100000;i++)printf("%c",keyspace[i]);
	//User Key MD5 berechnen
	key=malloc(sizeof(unsigned char)*0x10);
	key=MD5(keyspace,0x100000,key);
	free(keyspace);
	//Ausgabe
	printf("USER KEY: ");
	for(i=0;i<0x10;i++)printf("%02hX ",key[i]);puts("\n");
	//Localized Key
	int esize=strlen(argv[2])>>1;//EngineID:Text (da HEX=>ASCII bloss 1/2 Laenge)
	unsigned char*eText=argv[2],*eID=malloc(sizeof(unsigned char)*esize);
	while(*eText!='\0'){
		if((*eText>='0')&&(*eText<='9'))*eID=(*eText-'0')<<4;
		if((*eText>='A')&&(*eText<='Z'))*eID=(*eText-'7')<<4;//-'A'+10=-'7'
		if((*eText>='a')&&(*eText<='z'))*eID=(*eText-'W')<<4;//-'a'+10=-'W'
		eText++;
		if((*eText>='0')&&(*eText<='9'))*eID|=(*eText-'0');
		if((*eText>='A')&&(*eText<='Z'))*eID|=(*eText-'7');//-'A'+10=-'7'
		if((*eText>='a')&&(*eText<='z'))*eID|=(*eText-'W');//-'a'+10=-'W'
		eID++;eText++;
	}
	eID-=esize;
	//Ausgabe
	//for(i=0;i<esize;i++)printf("%02hX ",eID[i]);puts("\n");
	//Konkatenieren, Groesse keylen+engineIDlen+keylen (16+esize+16=esize+32)
	keyspace=malloc(sizeof(unsigned char)*(esize+0x20));//Wird missbraucht...
	memcpy(keyspace,key,0x10);
	memcpy(keyspace+0x10,eID,esize);
	memcpy(keyspace+0x10+esize,key,0x10);
	//Ausgabe
	//for(i=0;i<(esize+0x20);i++)printf("%02hX ",keyspace[i]);puts("\n");
	//Localized Key MD5
	key=MD5(keyspace,0x20+esize,key);
	free(keyspace);
	//Ausgabe
	printf("LOCALIZED KEY: ");
	for(i=0;i<0x10;i++)printf("%02hX ",key[i]);puts("\n");
	//SALT holen
	if(strlen(argv[3])!=0x10){printf("Salt hat falsche Laenge.\n");return 1;}
	unsigned char*salt=malloc(sizeof(unsigned char)*8);
	eText=argv[3];//Missbrauche eText...
	while(*eText!='\0'){
		if((*eText>='0')&&(*eText<='9'))*salt=(*eText-'0')<<4;
		if((*eText>='A')&&(*eText<='Z'))*salt=(*eText-'7')<<4;//-'A'+10=-'7'
		if((*eText>='a')&&(*eText<='z'))*salt=(*eText-'W')<<4;//-'a'+10=-'W'
		eText++;
		if((*eText>='0')&&(*eText<='9'))*salt|=(*eText-'0');
		if((*eText>='A')&&(*eText<='Z'))*salt|=(*eText-'7');//-'A'+10=-'7'
		if((*eText>='a')&&(*eText<='z'))*salt|=(*eText-'W');//-'a'+10=-'W'
		salt++;eText++;
	}
	salt-=8;
	//Ausgabe
	//for(i=0;i<8;i++)printf("%02hX ",salt[i]);puts("\n");
	//Initialization Vector
	DES_cblock ivec;//typedef unsigned char des_cblock[8];
	for(i=0;i<8;i++)ivec[i]=salt[i]^key[i+8];
	//Ausgabe
	printf("Initialization Vector: ");
	for(i=0;i<8;i++)printf("%02hX ",ivec[i]);puts("\n");
	//Key Schedule, braucht nur die ersten 8 Byte
	des_key_schedule ks;
	if ((i=des_key_sched((C_Block*)key,ks))!=0){printf("Key error %d\n",i);return 1;}
	//Missbrauche bp
	bp=malloc(sizeof(unsigned char)*fsize);
	DES_ncbc_encrypt(fb,bp,fsize,&ks,(C_Block*)ivec,DES_DECRYPT);
	//Ausgabe, die gleich aussieht wie im Trac...
	for(i=0;i<fsize;i++){if((i%16)==0)puts("");else printf(" ");printf("%02hx",bp[i]);}


	puts("\n");
	return 0;
}
