#include <stdio.h>
#include <unistd.h>
#include <strings.h>
#include <stdlib.h>
#include <stdint.h>

enum sol_types{sol_type_number=0x00,sol_type_boolean=0x01,sol_type_string=0x02,sol_type_object=0x03,
							sol_type_array=0x08,sol_type_date=0x0B,sol_type_xml=0x0F,sol_type_class=0x10};

struct data_object_date_struct{
	double millisec;
	int16_t minutes;
};

struct sol_data_object{
	struct sol_data_object*parent,*prev,*next;
	uint16_t data_object_name_length;
	char*data_object_name;
	uint8_t data_object_type;
	union{
		double data_object_double;
		uint8_t data_object_boolean;
		struct{
			uint16_t data_object_string_length;
			char*data_object_string;
		};
		struct sol_data_object*data_object_data_object;
		struct{
			uint32_t data_object_array_length;//Laenge besagt, wieviele Elemente enthalten sind
			struct sol_data_object*data_object_array;
		};
		struct data_object_date_struct data_object_date;
		struct{
			uint16_t data_object_xml_length;
			char*data_object_xml;
		};
		struct sol_data_object*data_object_class;
	};
};

struct sol_header{
	uint32_t sol_length;
	uint32_t sol_true_length;
	uint16_t object_name_length;
	char*object_name;
	struct sol_data_object*sol_body;
};

int main(int argc,char*argv[]){
	if(argc<2){puts("Bitte geben Sie den Dateinamen an.");return 1;}
	FILE*sol_fp=fopen(argv[1],"r");
	if(sol_fp==NULL){puts("Datei konnte nicht geoeffnet werden.");return 2;}
	//Header anfang
	if((getc(sol_fp)!=0)||(getc(sol_fp)!=0xbf)){//Magic value 0x00bf
		puts("Datei ist keine .sol-Datei.");return 3;
	}
	struct sol_header header;
	char*data_pointer;
	uint32_t filepos,loadpos;
	header.sol_length=(((((getc(sol_fp)<<8)|getc(sol_fp))<<8)|getc(sol_fp))<<8)|getc(sol_fp);
	header.sol_true_length=header.sol_length;
	if((getc(sol_fp)!='T')||(getc(sol_fp)!='C')||(getc(sol_fp)!='S')||(getc(sol_fp)!='O')){//TCSO
		puts("Datei ist keine .sol-Datei.");return 3;
	}
	if((getc(sol_fp)!=0)||(getc(sol_fp)!=0x04)||(getc(sol_fp)!=0)
		||(getc(sol_fp)!=0)||(getc(sol_fp)!=0)||(getc(sol_fp)!=0)){//0×000400000000
		puts("Datei ist keine .sol-Datei.");return 3;
	}
	//filepos=16;
	header.object_name_length=(getc(sol_fp)<<8)|getc(sol_fp);
	filepos=18;
	if((header.object_name_length==0)||(header.object_name_length>header.sol_true_length-filepos)){
		//Frees/Close/etc. fehlen...dirty
		puts("Groesse des Solobjektnamens stimmt nicht!");return 3;
	}
	header.object_name=malloc(sizeof(char)*(header.object_name_length+1));//==>'\000'
	data_pointer=header.object_name;
	for(loadpos=0;loadpos<header.object_name_length;loadpos++,data_pointer++)*data_pointer=getc(sol_fp);
	*data_pointer='\000';
	filepos+=loadpos;
	//Entferne Pads
	if((getc(sol_fp)!=0)||(getc(sol_fp)!=0)||(getc(sol_fp)!=0)||(getc(sol_fp)!=0)){
		//Frees/Close/etc. fehlen...dirty
		puts("Padding fehlt bei einem Objekt.");return 3;
	}
	//Header abgeschlossen
	//Body anfang
	header.sol_body=malloc(sizeof(struct sol_data_object));
	header.sol_body->parent=header.sol_body->prev=NULL;
	struct sol_data_object*current_object=header.sol_body;//Dirty :)
	while(filepos<header.sol_true_length){
		current_object->next=NULL;
		//Lade Groesse
		current_object->data_object_name_length=(getc(sol_fp)<<8)|getc(sol_fp);
		filepos+=2;
		if(filepos==0){//0x000009 moeglich, sonst Fehler
			if(getc(sol_fp)!=0x09){
				//Frees/Close/etc. fehlen...dirty
				puts("Groesse eines Objektnamens stimmt nicht!");return 3;
			}
			filepos++;
			//Unterpunkt zu Ende
			//Vater vom Current Object ist zu aendern
			if(current_object->parent!=NULL){
				switch(current_object->parent->data_object_type){
					case sol_type_object:
							if(current_object->parent->data_object_data_object==current_object)current_object->parent->data_object_data_object=NULL;
						break;
					case sol_type_array:
							if(current_object->parent->data_object_array==current_object)current_object->parent->data_object_array=NULL;
						break;
					case sol_type_class:
							if(current_object->parent->data_object_class==current_object)current_object->parent->data_object_class=NULL;
						break;
					//case...
					default:break;
				}
				current_object->parent=current_object->parent->parent;
			}
			if(current_object->prev!=NULL){
				current_object->prev->next=NULL;
				current_object->prev=NULL;
			}
			continue;
		}
		if(current_object->data_object_name_length>header.sol_true_length-filepos){
			//Frees/Close/etc. fehlen...dirty
			puts("Groesse eines Objektnamens stimmt nicht!");return 3;
		}
		//Lade Name
		current_object->data_object_name=malloc(sizeof(char)*(current_object->data_object_name_length+1));//==>'\000'
		data_pointer=current_object->data_object_name;
		for(loadpos=0;loadpos<current_object->data_object_name_length;loadpos++,data_pointer++)*data_pointer=getc(sol_fp);
		*data_pointer='\000';
		filepos+=loadpos;
		printf("%hu %s",current_object->data_object_name_length,current_object->data_object_name);
		//Entferne Pads
		/*if((getc(sol_fp)!=0)||(getc(sol_fp)!=0)||(getc(sol_fp)!=0)||(getc(sol_fp)!=0)){
			//Frees/Close/etc. fehlen...dirty
			puts("Padding fehlt bei einem Objekt.");return 3;
		}*/
		//Lade Inhalt
		current_object->data_object_type=getc(sol_fp);
		//Wichtiges Makro
		#define MAKE_NEXT_DATA_OBJECT \
			current_object->next=malloc(sizeof(struct sol_data_object));\
			current_object->next->parent=current_object->parent;\
			current_object->next->prev=current_object;\
			current_object=current_object->next;
		//Los gehts
		switch(current_object->data_object_type){
			case sol_type_number:
					current_object->data_object_double=0;
					data_pointer=((char*)&current_object->data_object_double)+7;//Boese, ich weis
					*data_pointer=getc(sol_fp);data_pointer--;
					*data_pointer=getc(sol_fp);data_pointer--;
					*data_pointer=getc(sol_fp);data_pointer--;
					*data_pointer=getc(sol_fp);data_pointer--;
					*data_pointer=getc(sol_fp);data_pointer--;
					*data_pointer=getc(sol_fp);data_pointer--;
					*data_pointer=getc(sol_fp);data_pointer--;
					*data_pointer=getc(sol_fp);
					filepos+=8;
					MAKE_NEXT_DATA_OBJECT
				break;
			case sol_type_boolean:
					current_object->data_object_boolean=(getc(sol_fp)==0x01);
					filepos+=1;
					MAKE_NEXT_DATA_OBJECT
				break;
			case sol_type_string:
					current_object->data_object_string_length=(getc(sol_fp)<<8)|getc(sol_fp);
					filepos+=2;
					if((current_object->data_object_string_length==0)||(current_object->data_object_string_length>header.sol_true_length-filepos)){
						//Frees/Close/etc. fehlen...dirty
						puts("Groesse eines Strings stimmt nicht!");return 3;
					}
					//Lade String
					current_object->data_object_string=malloc(sizeof(char)*(current_object->data_object_string_length+1));//==>'\000'
					data_pointer=current_object->data_object_string;
					for(loadpos=0;loadpos<current_object->data_object_string_length;loadpos++,data_pointer++)*data_pointer=getc(sol_fp);
					*data_pointer='\000';
					filepos+=loadpos;
					MAKE_NEXT_DATA_OBJECT
				break;
			case sol_type_object:
					current_object->data_object_data_object=malloc(sizeof(struct sol_data_object));
					current_object->data_object_data_object->parent=current_object;
					current_object=current_object->data_object_data_object;
					current_object->prev=NULL;
				break;
			case sol_type_array:
					current_object->data_object_array_length=(((((getc(sol_fp)<<8)|getc(sol_fp))<<8)|getc(sol_fp))<<8)|getc(sol_fp);
					filepos+=4;
					//Ginge auch anders...tue mal so als ob es kein Array waere, dirty!
					current_object->data_object_array=malloc(sizeof(struct sol_data_object));
					current_object->data_object_array->parent=current_object;
					current_object=current_object->data_object_array;
					current_object->prev=NULL;
				break;
			case sol_type_date:
					current_object->data_object_date.millisec=0;
					data_pointer=((char*)&current_object->data_object_date.millisec)+7;//Boese, ich weis
					*data_pointer=getc(sol_fp);data_pointer--;
					*data_pointer=getc(sol_fp);data_pointer--;
					*data_pointer=getc(sol_fp);data_pointer--;
					*data_pointer=getc(sol_fp);data_pointer--;
					*data_pointer=getc(sol_fp);data_pointer--;
					*data_pointer=getc(sol_fp);data_pointer--;
					*data_pointer=getc(sol_fp);data_pointer--;
					*data_pointer=getc(sol_fp);
					current_object->data_object_date.minutes=(getc(sol_fp)<<8)|getc(sol_fp);
					filepos+=10;
					MAKE_NEXT_DATA_OBJECT
				break;
			case sol_type_xml:
					if(getc(sol_fp)!=0x00){
						//Frees/Close/etc. fehlen...dirty
						puts("XML-Objekt fehlerhaft!");return 3;
					}
					current_object->data_object_xml_length=(getc(sol_fp)<<8)|getc(sol_fp);
					filepos+=2;
					if((current_object->data_object_xml_length==0)||(current_object->data_object_xml_length>header.sol_true_length-filepos)){
						//Frees/Close/etc. fehlen...dirty
						puts("Groesse eines XML-Objektes stimmt nicht!");return 3;
					}
					//Lade XML
					current_object->data_object_xml=malloc(sizeof(char)*(current_object->data_object_xml_length+1));//==>'\000'
					data_pointer=current_object->data_object_xml;
					for(loadpos=0;loadpos<current_object->data_object_xml_length;loadpos++,data_pointer++)*data_pointer=getc(sol_fp);
					*data_pointer='\000';
					filepos+=loadpos;
					MAKE_NEXT_DATA_OBJECT
				break;
			case sol_type_class:
					current_object->data_object_class=malloc(sizeof(struct sol_data_object));
					current_object->data_object_class->parent=current_object;
					current_object=current_object->data_object_class;
					current_object->prev=NULL;
				break;
			default:break;
		}
	}
	//Letzten immer loeschen
	if(current_object==header.sol_body)header.sol_body=NULL;
	if(current_object->parent!=NULL){
		switch(current_object->parent->data_object_type){
			case sol_type_object:
					if(current_object->parent->data_object_data_object==current_object)current_object->parent->data_object_data_object=NULL;
				break;
			case sol_type_array:
					if(current_object->parent->data_object_array==current_object)current_object->parent->data_object_array=NULL;
				break;
			case sol_type_class:
					if(current_object->parent->data_object_class==current_object)current_object->parent->data_object_class=NULL;
				break;
			//case...
			default:break;
		}
	}
	if(current_object->prev!=NULL)current_object->prev->next=NULL;
	free(current_object);
	//Body abgeschlossen
	printf("Filepos: %u\n",filepos);
	printf("Laenge %u, Name %s, Laenge des Namens %u\n",header.sol_length,header.object_name,header.object_name_length);
	free(header.object_name);
	fclose(sol_fp);
	return 0;
}
