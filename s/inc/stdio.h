
#if defined(is_main_c)||defined(is_split_c)
#ifdef __cplusplus
extern "C" {
#endif
int puts(const char*);
#ifdef __cplusplus
}
#endif
#endif

#if defined(is_main_c)||defined(is_bar_c)
#ifdef __cplusplus
extern "C" {
#endif
int sprintf(char*,const char*,...);
#ifdef __cplusplus
}
#endif
#endif
