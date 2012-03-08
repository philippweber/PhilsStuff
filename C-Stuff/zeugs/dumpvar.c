#include<stdio.h>
#include<stdlib.h>
#define DUMP(varname) fprintf(stderr, "%s = %X\n", #varname, varname);
//#define DUMP(varname) fprintf(stderr, #varname" = %X\n", varname); //Geht auch
int main(int argc,char*argv[]){
        int test=42;
        DUMP(test)
        return EXIT_SUCCESS;
}
