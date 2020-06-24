
#ifndef __cplusplus

typedef char bool;
enum{false=0,true=1};
#define nullptr 0

#endif

typedef struct{
	char*data;
	unsigned int spc;
	unsigned int sz;
}row;
