#include <stdio.h>
#include <unistd.h>
#include <strings.h>
#include <stdlib.h>

char sol_object(FILE*sol_fp);

char sol_string(FILE*sol_fp){
	puts("String angefangen");
	unsigned int sol_string_length=0,counter;//Laenge, Zaehler
	char*sol_string_name;//Name
	sol_string_length=(((unsigned int)getc(sol_fp))<<8)|((unsigned int)getc(sol_fp));//Erste 2 Bytes=Laenge
	if(sol_string_length==0){puts("String fehlerhaft oder Datei zu Ende(==>Quick&Dirty).");return 0;}
	printf("Der Name des String sollte %u lang sein.\n",sol_string_length);//Laenge
	sol_string_name=malloc(sizeof(char)*(sol_string_length+1));//Fuer NULL
	for(counter=0;counter<sol_string_length;counter++){
		if((sol_string_name[counter]=getc(sol_fp))==EOF){
			puts("String fehlerhaft, Datei zu Ende.");
			return 0;
		}
	}
	sol_string_name[sol_string_length]='\000';//NULL
	puts("String abgeschlossen");
	puts(sol_string_name);
	free(sol_string_name);
	return 1;
}

char sol_data(FILE*sol_fp){
	puts("Variable angefangen");
	unsigned int sol_var_length=0,counter;//Laenge, Zaehler
	char*sol_var_name;//Name
	//Datentypen
	double sol_typ_d;
	//char sol_var_type;//Typ
	sol_var_length=(((unsigned int)getc(sol_fp))<<8)|((unsigned int)getc(sol_fp));//Erste 2 Bytes=Laenge
	if(sol_var_length==0){puts("Variable fehlerhaft oder Datei zu Ende(==>Quick&Dirty).");return 0;}
	printf("Der Name der Variable sollte %u lang sein.\n",sol_var_length);//Laenge
	sol_var_name=malloc(sizeof(char)*(sol_var_length+1));//Fuer NULL
	for(counter=0;counter<sol_var_length;counter++){
		if((sol_var_name[counter]=getc(sol_fp))==EOF){
			puts("Variable fehlerhaft, Datei zu Ende.");
			return 0;
		}
	}
	sol_var_name[sol_var_length]='\000';//NULL
	puts(sol_var_name);
	switch(getc(sol_fp)){//Typ
		case 0x00:puts("Double");fscanf(sol_fp,"%lf",&sol_typ_d);printf("%lf\n",sol_typ_d);break;//NUMBER.This is a double number so the next eight bytes represent the number.
		case 0x01:puts("Boolean ");if(getc(sol_fp)==0)puts("False");else puts("True");break;//BOOLEAN. One byte is read, either it is TRUE (value of 0×01) or FALSE (value of 0×00).
		case 0x02:puts("String");if(sol_string(sol_fp)==0){puts("Variable fehlerhaft");return 0;}break;//STRING. Contains two bytes representing the length of the string, followed by that amount of bytes in ASCII.
		case 0x03:puts("OBJECT");if(sol_object(sol_fp)==0){puts("Variable fehlerhaft");return 0;}break;//OBJECT. An object contains various data types wrapped inside a single data type.  It has the same structure as other data types, that is it begins with two bytes representing the length of the name part of the variable, followed by the name in ASCII and then one byte representing the data type, which is then read the same way as any other data type. is read.  The object then ends with the magic number of “0×00 00 09”.
		case 0x05:puts("NULL OBJECT");break;//NULL.  This is just a null object.
		case 0x06:puts("UNDEFINED");break;//UNDEFINED.  This is also a null object, since it has not been defined.
		case 0x08:puts("ARRAY");break;//ARRAY. The array contains the exactly the same structure as an object, except that it starts with four bytes representing the number of elements stored inside the array.  As with the objects it ends with the magic number of “0×00 00 09”.
		case 0x0B:puts("DATE");break;//DATE. A date object is ten bytes, beginning with a double number contained in the first eight bytes followed by a signed short integer (two bytes) that represents the time offset in minutes from Epoch time.  The double number represents an Epoch time in milliseconds.
		case 0x0D:puts("NULL");break;//NULL. As the name implies a null value.
		case 0x0F:puts("XML");break;//XML. An XML structure begins with two bytes that should equal zero (0×00) followed by a string value (first two bytes represent the length of the XML, then an ASCII text containing the XML text).
		case 0x10:puts("CLASS");break;//CLASS. The class is very similar to that of an array or an object.  The only difference is a class starts with a name of the class.  The first two bytes represent the length of the name, followed by the name of the class in ASCII.  Then as usual additional data types that are contained within the class, ending with a magic value of “0×00 00 09”.
		default:puts("Unbekannte Variablenart!");return 0;break;
	}
	puts(sol_var_name);
	puts("Variable abgeschlossen");
	free(sol_var_name);
	return 1;
}

char sol_object(FILE*sol_fp){
	puts("Objekt angefangen");
	unsigned int sol_obj_length=0,counter;//Laenge, Zaehler
	char*sol_obj_name;//Name
	sol_obj_length=(((unsigned int)getc(sol_fp))<<8)|((unsigned int)getc(sol_fp));//Erste 2 Bytes=Laenge
	if(sol_obj_length==0){puts("Objekt fehlerhaft oder Datei zu Ende(==>Quick&Dirty).");return 0;}
	printf("Der Name des Objekts sollte %u lang sein.\n",sol_obj_length);//Laenge
	sol_obj_name=malloc(sizeof(char)*(sol_obj_length+1));//Fuer NULL
	for(counter=0;counter<sol_obj_length;counter++){
		if((sol_obj_name[counter]=getc(sol_fp))==EOF){
			puts("Objekt fehlerhaft, Datei zu Ende.");
			return 0;
		}
	}
	sol_obj_name[sol_obj_length]='\000';//NULL
	puts(sol_obj_name);
	if((getc(sol_fp)!=0)||(getc(sol_fp)!=0)||(getc(sol_fp)!=0)||(getc(sol_fp)!=0)){//0x00000000
		puts("Objekt fehlerhaft.");
		return 0;
	}
	while(sol_data(sol_fp));
	puts(sol_obj_name);
	puts("Objekt abgeschlossen");
	free(sol_obj_name);
	return 1;
}

int main(int argc,char*argv[]){
	if(argc<2){puts("Bitte geben Sie den Dateinamen an.");return 1;}
	FILE*sol_fp=fopen(argv[1],"r");
	if(sol_fp==NULL){puts("Datei konnte nicht geoeffnet werden.");return 2;}
	if((getc(sol_fp)!=0)||(getc(sol_fp)!=0xbf)){//Magic value 0x00bf
		puts("Datei ist keine .sol-Datei.");return 3;
	}
	puts("0x00BF");
	int sol_length;
	sol_length=getw(sol_fp);
	printf("Die .sol-Datei sollte %d lang sein.\n",sol_length);//Laenge
	if((getc(sol_fp)!='T')||(getc(sol_fp)!='C')||(getc(sol_fp)!='S')||(getc(sol_fp)!='O')){//TCSO
		puts("Datei ist keine .sol-Datei.");return 3;
	}
	puts("TSCO");
	if((getc(sol_fp)!=0)||(getc(sol_fp)!=0x04)||(getc(sol_fp)!=0)
		||(getc(sol_fp)!=0)||(getc(sol_fp)!=0)||(getc(sol_fp)!=0)){//0×000400000000
		puts("Datei ist keine .sol-Datei.");return 3;
	}
	puts("0×000400000000");
	sol_object(sol_fp);
	fclose(sol_fp);
	return 0;
}
