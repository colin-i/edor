
typedef char bool;
enum{false=0,true=1};

//#include <string.h>
typedef unsigned int size_t;
size_t strlen(const char*);
//#include <fcntl.h>
int open(const char*,int,...);
//asm-generic/fcntl.h
#define O_WRONLY 00000001
#define O_TRUNC 00001000
//#include <unistd.h>
int close(int);
typedef int ssize_t;
ssize_t write(int,const void*,size_t);
//#include <stdlib.h>
void*malloc(size_t);
void free(void*);

bool out_f(char*);