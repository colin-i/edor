
typedef unsigned int size_t;
//asm-generic/fcntl.h
#define O_WRONLY 00000001
#define O_TRUNC 00001000
//
typedef int ssize_t;

#ifdef __cplusplus
extern "C" {
#endif

//#include <string.h>
int strcmp(const char*,const char*);
void*memcpy(void*,const void*,size_t);
size_t strlen(const char*);
//stdio.h
int sprintf(char*,const char*,...);
//#include <fcntl.h>
int open(const char*,int,...);
//#include <unistd.h>
int close(int);
ssize_t write(int,const void*,size_t);
//#include <stdlib.h>
void free(void*);
void*realloc(void*,size_t);
void*malloc(size_t);

#ifdef __cplusplus
}
#else

typedef char bool;
enum{false=0,true=1};
#define nullptr 0

#endif

typedef struct{
	char*data;
	size_t spc;
	size_t sz;
}row;
