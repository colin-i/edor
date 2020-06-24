
#if defined(is_main_c)||defined(is_bar_c)
#include "inc/stddef.h"
#ifdef __cplusplus
extern "C" {
#endif
void free(void*);
void*realloc(void*,size_t);
void*malloc(size_t);
#ifdef __cplusplus
}
#endif
#endif

#if defined(is_main_c)||defined(is_sep_c)
#ifdef __cplusplus
extern "C" {
#endif
char*getenv(const char*);
#ifdef __cplusplus
}
#endif
#endif
