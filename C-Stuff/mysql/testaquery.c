#include <stddef.h>
#include <stdio.h>
#include <mysql/mysql.h>

int main(int argc,char*argv[]){
	//Mit Datenbank verbinden
	MYSQL*conn;
	MYSQL_RES*res;
	MYSQL_ROW row;
	conn=mysql_init(NULL);
	//Connect to database
	if(!mysql_real_connect(conn,"1","1","1","1",1,NULL,0)){
		fprintf(stderr,"%s\n",mysql_error(conn));
		return 1;
	}
	//Charset
	#define charset "utf8" //Character Set
	if(mysql_query(conn,"SET NAMES '"charset"'")){
		fprintf(stderr,"%s\n",mysql_error(conn));
		mysql_close(conn);
		return 1;
	}
	//Hier testen
	#define rules_modules_table "`sdb_traplistener_rules_modules`" //Enthaelt die Verknuepfung zwischen den Regeln und den Modulen (das erzeugt das eigentliche Set)
	#define rules_modules_table_priority_column rules_modules_table".`rm_rule_priority`" //Die Prioritaet der Regel/Modul-Verknuepfung
	if(mysql_query(conn,"SELECT MAX("rules_modules_table_priority_column") FROM "rules_modules_table)){
	//if(mysql_query(conn,"SELECT in_bytes FROM sdb_traffic_ip_tage WHERE ip='0000:0000:0000:0000:0000:FFFF:88C7:0B79' AND datum=curdate() AND srcid = '1'")){
		fprintf(stderr,mysql_error(conn));
	}else if((res=mysql_use_result(conn))==NULL){
		fprintf(stderr,mysql_error(conn));
	}else	if((row=mysql_fetch_row(res))==NULL){/* wird im Makro letzte Zeile gemacht*/
		fprintf(stderr,mysql_error(conn));
		mysql_free_result(res);
	}else{
		puts(row[0]);
		mysql_free_result(res);
	}
	mysql_close(conn);
	return 0;//EXIT_SUCCESS;
}
