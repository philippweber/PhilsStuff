#include <ulimit.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

int main(int argc,char*argv[]){
	long int value;
	long int limit;

	/* Get the file size resource limit. */
	value = ulimit(UL_GETFSIZE);
	if ((value == -1) && (errno != 0)) {
		printf("ulimit() failed with errno=%d\n", errno);
		exit(1);
	}
	printf("The limit is %ld\n", value);

	/* Set the file size resource limit. */
	value = ulimit(UL_SETFSIZE, limit);
	if ((value == -1) && (errno != 0)) {
		printf("ulimit() failed with errno=%d\n", errno);
		exit(1);
	}
	printf("The limit is set to %ld\n", value);

	exit(0);
}
