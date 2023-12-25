
#if defined(is_main_c)||defined(is_bar_c)||defined(is_sep_c)
#include "inc/stddef.h"
#ifdef __cplusplus
extern "C" {
#endif
size_t strlen(const char*);
#ifdef __cplusplus
}
#endif
#endif

#if defined(is_main_c)||defined(is_bar_c)
#ifdef __cplusplus
extern "C" {
#endif
int strcmp(const char*,const char*);
void*memcpy(void*,const void*,size_t);
#ifdef __cplusplus
}
#endif
#endif

#if defined(is_bar_c)||defined(is_sep_c)
#ifdef __cplusplus
extern "C" {
#endif
char*strrchr(const char*,int);
#ifdef __cplusplus
}
#endif
#endif
