
//asm-generic/fcntl.h
#define O_WRONLY 00000001
#define O_TRUNC 00001000

#ifdef __cplusplus
extern "C" {
#endif

//#include <fcntl.h>
int open(const char*,int,...);

#ifdef __cplusplus
}
#else

typedef char bool;
enum{false=0,true=1};
#define nullptr 0

#endif

typedef struct{
	char*data;
	unsigned int spc;
	unsigned int sz;
}row;
