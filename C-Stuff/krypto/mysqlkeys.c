/***
 * Programm welches die Schluessel lokalisiert
 * */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql/mysql.h>
#include <openssl/md5.h>

int main(){
	MYSQL *conn_select,*conn_update;
	MYSQL_RES *res;//Nur fuer SELECT, UPDATE gibt kein Resultat zurueck
	MYSQL_ROW row;//Nur fuer SELECT
	//Settings
	char*server="1";
	char*user="1";
	char*password="1";
	char*database="1";
	int port=1;
	//Init
	conn_select=mysql_init(NULL);
	conn_update=mysql_init(NULL);
	//Connect to database
	if(!mysql_real_connect(conn_select,server,user,password,database,port,NULL,0)){
		fprintf(stderr,"%s\n",mysql_error(conn_select));
		return 1;
	}
	if(!mysql_real_connect(conn_update,server,user,password,database,port,NULL,0)){
		fprintf(stderr,"%s\n",mysql_error(conn_update));
		return 1;
	}
	//Hole alle Passwoerter
	if(mysql_query(conn_select,"SELECT `hostname`,`snmpEngineID`,unhex(`snmpEngineID`),`snmp_auth_passphrase`,`snmp_priv_passphrase` \
FROM `sdb_switches` WHERE `snmp_auth_passphrase`!=\"\" OR `snmp_priv_passphrase`!=\"\"")){
		fprintf(stderr,"%s\n",mysql_error(conn_select));
		return 1;
	}
	printf("Relevante Eintraege in 'sdb_switches':\n");
	puts("");
	res=mysql_use_result(conn_select);
	//Speicher fuer Schluessel,etc.
	unsigned long*row_data_len;//Laenge der Daten von Rows
	unsigned long eIDlen;//Laenge der EngineID (Row[2]), eigentlich reicht ein char oder int
	unsigned char*keyspace=malloc(sizeof(unsigned char)*0x100000);//1 MB fuer den User Key Algorithmus
	unsigned char*auth_user_key=malloc(sizeof(unsigned char)*0x10);//User Auth Key
	//unsigned char*auth_user_key_hex=malloc(sizeof(unsigned char)*0x21);//User Auth Key Hex
	unsigned char*auth_local_key=malloc(sizeof(unsigned char)*0x10);//Local Auth Key
	//unsigned char*auth_local_key_hex=malloc(sizeof(unsigned char)*0x21);//Local Auth Key Hex
	unsigned char*priv_user_key=malloc(sizeof(unsigned char)*0x10);//User Priv Key
	//unsigned char*priv_user_key_hex=malloc(sizeof(unsigned char)*0x21);//User Priv Key Hex
	unsigned char*priv_local_key=malloc(sizeof(unsigned char)*0x10);//Local Priv Key
	//unsigned char*priv_local_key_hex=malloc(sizeof(unsigned char)*0x21);//Local Priv Key Hex
	unsigned char*kp1,*kp2;//Pointer auf Schluessel
	//Update-Query initialisieren:
	//Das Query setzt sich zusammen aus:
	//"UPDATE `sdb_switches` SET `snmp_auth_user_key`=\""  ==> Laenge: 48
	//"\",`snmp_auth_localized_key`=\""  ==> Laenge: 29
	//"\",`snmp_priv_user_key`=\""  ==> Laenge: 24
	//"\",`snmp_priv_localized_key`=\""  ==> Laenge: 29
	//"\" WHERE `hostname`=\""  ==> Laenge: 20 //Nur hostname pruefen da primary => unique :)
	//"\";"  ==> Laenge: 2
	//Gesamtlaenge: 152 (konstant)
	//auth_user_key ==> Laenge: 32
	//auth_local_key ==> Laenge: 32
	//priv_user_key ==> Laenge: 32
	//priv_local_key ==> Laenge: 32
	//Hostname maximal 255, row_data_len[0] genauer...
	//Gesamtlaenge: 152 + 128 + 255 == 535
	unsigned char*update_query=malloc(sizeof(unsigned char)*535);
	unsigned long update_query_len;//Laenge
	//Ausgabe und Berechnungen...
	while((row=mysql_fetch_row(res))!=NULL){
		//Ausgabe
		printf("Eintrag Nr.: %llu\n",mysql_num_rows(res));
		printf("Hostname ; EngineID ; Auth_Passphrase ; Priv_Passphrase ;\n");
		printf("%s ; %s ; %s ; %s ;\n",row[0],row[1],row[3],row[4]);
		puts("");
		//eIDlen=strlen(row[1])>>1;//Trick, Messe die Laenge von row[1] um auf die Laenge von row[2] zu kommen
		row_data_len=mysql_fetch_lengths(res);//Besser, strlen fehlerhaft
		eIDlen=row_data_len[2];
		if(eIDlen==0){
			fprintf(stderr,"Error in EngineID\n");
			continue;//Weitermachen mit naechster Row
		}
		//printf("%lu %lu %lu %lu %lu",row_data_len[0],row_data_len[1],row_data_len[2],row_data_len[3],row_data_len[4]);
		//mysql_hex_string() wuerds auch tun
		//int i;kp1=(unsigned char*)row[2];
		//for(i=0;i<eIDlen;i++)printf("%02hX \n",kp1[i]);
		//Auth_Passphrase vorhanden?
		if(row_data_len[3]!=0){//!strlen(row[3])
			//Berechne User Key von Auth
			kp2=(unsigned char*)row[3];
			for(kp1=keyspace;kp1<keyspace+0x100000;kp1++){
				if(*kp2=='\0')kp2=(unsigned char*)row[3];
				*kp1=*kp2;
				kp2++;
			}
			//Auth User Key MD5 berechnen
			//auth_key=MD5(keyspace,0x100000,auth_key);
			MD5(keyspace,0x100000,auth_user_key);
			//Ausgabe
			printf("AUTH USER KEY: ");
			for(kp1=auth_user_key;kp1<auth_user_key+0x10;kp1++)printf("%02hX ",*kp1);
			puts("");
			//Berechne Localized Key von Auth
			//Konkatenieren, Groesse key_length+engineID_length+key_length (16+eIDlen+16=eIDlen+32)
			kp1=malloc(sizeof(unsigned char)*(eIDlen+0x20));//Wird missbraucht...
			memcpy(kp1,auth_user_key,0x10);
			memcpy(kp1+0x10,row[2],eIDlen);
			memcpy(kp1+0x10+eIDlen,auth_user_key,0x10);
			//Localized Key MD5
			//auth_local_key=MD5(kp1,0x20+eIDlen,auth_local_key);
			MD5(kp1,0x20+eIDlen,auth_local_key);
			//Aufraeumen
			free(kp1);
			//Ausgabe
			printf("AUTH LOCALIZED KEY: ");
			for(kp1=auth_local_key;kp1<auth_local_key+0x10;kp1++)printf("%02hX ",*kp1);
			puts("");
		}
		//Priv_Passphrase vorhanden?
		if(row_data_len[4]!=0){//!strlen(row[4])
			//Berechne User Key von Priv
			kp2=(unsigned char*)row[4];
			for(kp1=keyspace;kp1<keyspace+0x100000;kp1++){
				if(*kp2=='\0')kp2=(unsigned char*)row[4];
				*kp1=*kp2;
				kp2++;
			}
			//Priv User Key MD5 berechnen
			//priv_key=MD5(keyspace,0x100000,priv_key);
			MD5(keyspace,0x100000,priv_user_key);
			//Ausgabe
			printf("PRIV USER KEY: ");
			for(kp1=priv_user_key;kp1<priv_user_key+0x10;kp1++)printf("%02hX ",*kp1);
			puts("");
			//Berechne Localized Key von Priv
			//Konkatenieren, Groesse key_length+engineID_length+key_length (16+eIDlen+16=eIDlen+32)
			kp1=malloc(sizeof(unsigned char)*(eIDlen+0x20));//Wird missbraucht...
			memcpy(kp1,priv_user_key,0x10);
			memcpy(kp1+0x10,row[2],eIDlen);
			memcpy(kp1+0x10+eIDlen,priv_user_key,0x10);
			//Localized Key MD5
			//auth_local_key=MD5(kp1,0x20+eIDlen,auth_local_key);
			MD5(kp1,0x20+eIDlen,priv_local_key);
			//Aufraeumen
			free(kp1);
			//Ausgabe
			printf("PRIV LOCALIZED KEY: ");
			for(kp1=priv_local_key;kp1<priv_local_key+0x10;kp1++)printf("%02hX ",*kp1);
			puts("");
		}
		puts("");
		//Update-Query initialisieren:
		//Das Query setzt sich zusammen aus:
		//"UPDATE `sdb_switches` SET `snmp_auth_user_key`=\""  ==> Laenge: 48
		//"\",`snmp_auth_localized_key`=\""  ==> Laenge: 29
		//"\",`snmp_priv_user_key`=\""  ==> Laenge: 24
		//"\",`snmp_priv_localized_key`=\""  ==> Laenge: 29
		//"\" WHERE `hostname`=\""  ==> Laenge: 20 //Nur hostname pruefen da primary => unique :)
		//"\";"  ==> Laenge: 2
		//Gesamtlaenge: 152 (konstant)
		//auth_user_key ==> Laenge: 32
		//auth_local_key ==> Laenge: 32
		//priv_user_key ==> Laenge: 32
		//priv_local_key ==> Laenge: 32
		//Hostname maximal 255, row_data_len[0] genauer...
		//Eigentlich muesste dieser Teil nur einmal gemacht werden, weil alle Schluessel konstante Groessen haben...
		//Es sei den diese sind leer...
		//Wenn Sie leer sind, wird der Wert in der Datenbank geloescht
		kp1=(unsigned char*)strcpy((char*)update_query,"UPDATE `sdb_switches` SET `snmp_auth_user_key`=\"");kp1+=48;
		if(row_data_len[3]!=0)kp1+=mysql_hex_string((char*)kp1,(char*)auth_user_key,0x10);//!=0x20 ==> Error...
		strcpy((char*)kp1,"\",`snmp_auth_localized_key`=\"");kp1+=29;
		if(row_data_len[3]!=0)kp1+=mysql_hex_string((char*)kp1,(char*)auth_local_key,0x10);//!=0x20 ==> Error...
		strcpy((char*)kp1,"\",`snmp_priv_user_key`=\"");kp1+=24;
		if(row_data_len[4]!=0)kp1+=mysql_hex_string((char*)kp1,(char*)priv_user_key,0x10);//!=0x20 ==> Error...
		strcpy((char*)kp1,"\",`snmp_priv_localized_key`=\"");;kp1+=29;
		if(row_data_len[4]!=0)kp1+=mysql_hex_string((char*)kp1,(char*)priv_local_key,0x10);//!=0x20 ==> Error...
		strcpy((char*)kp1,"\" WHERE `hostname`=\"");kp1+=20;
		strcpy((char*)kp1,(char*)row[0]);kp1+=row_data_len[0];
		strcpy((char*)kp1,"\";");kp1+=2;
		printf("%s \n",(char*)update_query);
		update_query_len=kp1-update_query;
		//Benutze  wegen der Laenge
		if(mysql_real_query(conn_update,(char*)update_query,update_query_len)){
			fprintf(stderr,"%s\n",mysql_error(conn_update));
			return 1;
		}
		printf("Rows updated: %llu, warnings: %u\n",mysql_affected_rows(conn_update),mysql_warning_count(conn_update));
		puts("\n");
	}
	printf("Insgesamt %llu Eintraege bearbeitet.\n\n",mysql_num_rows(res));
	//Aufraeumen
	free(keyspace);
	free(auth_user_key);
	free(auth_local_key);
	free(priv_user_key);
	free(priv_local_key);
	free(update_query);
	mysql_free_result(res);
	//Close connection
	mysql_close(conn_update);
	mysql_close(conn_select);
	return 0;
}
