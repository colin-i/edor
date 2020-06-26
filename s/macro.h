
#ifndef __cplusplus

#ifdef HAVE_STDBOOL_H
#include<stdbool.h>
#else
typedef char bool;
enum{false=0,true=1};
#endif

#define nullptr 0

#endif

typedef struct{
	char*data;
	unsigned int spc;
	unsigned int sz;
}row;
