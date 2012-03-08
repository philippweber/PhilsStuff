#include <limits.h>
#include <stdio.h>
int main(int argc,char*argv[]){
	#ifdef ULLONG_MAX
		printf("ULLONG_MAX: %llu, ",ULLONG_MAX);
	#endif
	#ifdef LLONG_MAX
		printf("LLONG_MAX: %lld, ",LLONG_MAX);
	#endif
	#ifdef LLONG_MIN
		printf("LLONG_MIN: %lld, ",LLONG_MIN);
	#endif
	puts("");
	#ifdef ULONG_MAX
		printf("ULONG_MAX: %lu, ",ULONG_MAX);
	#endif
	#ifdef LONG_MAX
		printf("LONG_MAX: %ld, ",LONG_MAX);
	#endif
	#ifdef LONG_MIN
		printf("LONG_MIN: %ld, ",LONG_MIN);
	#endif
	puts("");
	#ifdef UINT_MAX
		printf("UINT_MAX: %u, ",UINT_MAX);
	#endif
	#ifdef INT_MAX
		printf("INT_MAX: %d, ",INT_MAX);
	#endif
	#ifdef INT_MIN
		printf("INT_MIN: %d, ",INT_MIN);
	#endif
	puts("");
	#ifdef USHRT_MAX
		printf("USHRT_MAX: %u, ",USHRT_MAX);
	#endif
	#ifdef SHRT_MAX
		printf("SHRT_MAX: %d, ",SHRT_MAX);
	#endif
	#ifdef SHRT_MIN
		printf("SHRT_MIN: %d, ",SHRT_MIN);
	#endif
	puts("");
	#ifdef UCHAR_MAX
		printf("UCHAR_MAX: %u, ",UCHAR_MAX);
	#endif
	#ifdef CHAR_MAX
		printf("CHAR_MAX: %d, ",CHAR_MAX);
	#endif
	#ifdef CHAR_MIN
		printf("CHAR_MIN: %d, ",CHAR_MIN);
	#endif
	#ifdef CHAR_BIT
		printf("CHAR_BIT: %u, ",CHAR_BIT);
	#endif
	puts("");
	return 0;
}
