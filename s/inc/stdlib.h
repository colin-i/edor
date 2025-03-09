
#if defined(is_main_c)||defined(is_bar_c)||defined(is_tw_c)||defined(is_tit_c)||defined(is_split_c)
//#include "inc/stddef.h"
#ifdef __cplusplus
extern "C" {
#endif
void*malloc(size_t);
#ifdef __cplusplus
}
#endif
#endif

#if defined(is_main_c)||defined(is_bar_c)||defined(is_tw_c)||defined(is_split_c)
#ifdef __cplusplus
extern "C" {
#endif
void free(void*);
#ifdef __cplusplus
}
#endif
#endif

#if defined(is_main_c)||defined(is_bar_c)
#ifdef __cplusplus
extern "C" {
#endif
void*realloc(void*,size_t);
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
