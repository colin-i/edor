
#ifndef __cplusplus

#ifdef HAVE_STDBOOL_H
#include<stdbool.h>
#else
#ifdef _Bool
typedef typeof(_Bool) bool;
#else
typedef char bool;
#endif
enum{false=0!=0,true=1==1};
#endif

#include "null.h"

#endif

typedef struct{
	char*data;
	unsigned int spc;
	unsigned int sz;
}row;
