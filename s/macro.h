
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

#ifdef HAVE_STDDEF_H
#include<stddef.h>
//there are warnings at cpp with NULL from stddef at comparisons
#define nullptr NULL
#else
#define nullptr ((void*)0)
#endif

#endif

typedef struct{
	char*data;
	unsigned int spc;
	unsigned int sz;
}row;
