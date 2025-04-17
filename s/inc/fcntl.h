
#if defined(is_main_c)||defined(is_bar_c)
#define O_CREAT 00000100

//linux/stat.h
#define S_IRUSR 00400
#define S_IWUSR 00200
#endif

#if defined(is_main_c)||defined(is_split_c)
#define O_RDONLY 00000000
#endif

#ifdef __cplusplus
extern "C" {
#endif

//typedef unsigned short mode_t;3rd argument
int open(const char*,int,...);

#ifdef __cplusplus
}
#endif
