
#define is_main_c
#include "inc/string.h"

#ifdef __cplusplus
extern "C" {
#endif

void*memset(void*,int,size_t);
char *strcpy(char *restrict dst, const char *restrict src);
char *strdup(const char *s);

#ifdef __cplusplus
}
#endif
