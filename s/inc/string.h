
#if defined(is_main_c)||defined(is_bar_c)||defined(is_sep_c)||defined(is_split_c)
//#include "inc/stddef.h"
#ifdef __cplusplus
extern "C" {
#endif
size_t strlen(const char*);
#ifdef __cplusplus
}
#endif
#endif

#if defined(is_main_c)||defined(is_bar_c)||defined(is_tit_c)
#ifdef __cplusplus
extern "C" {
#endif
int strcmp(const char*,const char*);
#ifdef __cplusplus
}
#endif
#endif

#if defined(is_main_c)||defined(is_bar_c)||defined(is_split_c)
#ifdef __cplusplus
extern "C" {
#endif
void*memcpy(void*,const void*,size_t);
#ifdef __cplusplus
}
#endif
#endif

#if defined(is_main_c)||defined(is_tit_c)||defined(is_split_c)
#ifdef __cplusplus
extern "C" {
#endif
int memcmp(const void*,const void*,size_t);
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
