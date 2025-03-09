
#if defined(is_main_c)||defined(is_bar_c)||defined(is_split_c)
typedef long int ssize_t;
#endif

#if defined(is_main_c)||defined(is_bar_c)
#define F_OK 0
//#include "inc/stddef.h"
#ifdef __cplusplus
extern "C" {
#endif
int access(const char*,int);
int close(int);
ssize_t write(int,const void*,size_t);
#ifdef __cplusplus
}
#endif
#endif

#if defined(is_main_c)||defined(is_split_c)
//sys/types.h
typedef long off_t;
//#include <bits/seek_constants.h>
#define SEEK_SET 0
#define SEEK_END 2
#ifdef __cplusplus
extern "C" {
#endif
off_t lseek(int,off_t,int);
ssize_t read(int,void*,size_t);
#ifdef __cplusplus
}
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif
int close(int);
#ifdef __cplusplus
}
#endif
