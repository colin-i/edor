
#include "inc/string.h"

#ifdef __cplusplus
extern "C" {
#endif

void*memchr(void*,int,size_t);
#ifdef HAVE_MEMMEM  //hard to happen
void *memmem(const void*,size_t,const void*,size_t);//this is provided in our split.c
#endif

#ifdef __cplusplus
}
#endif
