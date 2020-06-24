
#include"inc/stddef.h"

#ifdef __cplusplus
extern "C" {
#endif

int strcmp(const char*,const char*);
void*memcpy(void*,const void*,size_t);
size_t strlen(const char*);

char*strerror(int);

#ifdef __cplusplus
}
#endif
