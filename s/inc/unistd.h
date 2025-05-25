
//#if defined(is_main_c)||defined(is_bar_c)||defined(is_split_c)

//this is at sys/types.h
#undef platform_is_known
#ifdef SIZEOF_INT_P
#	if SIZEOF_INT_P >= 8
#		ifdef SIZEOF_LONG
#			define platform_is_known 1
#			if SIZEOF_LONG == 4
				typedef long long int ssize_t; //long long for windows
#			else
				typedef long int ssize_t;
#			endif
#		endif
#	endif
#endif
#ifndef platform_is_known
	typedef int ssize_t;
#else
#	undef platform_is_known
#endif

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
//#endif

#if defined(is_main_c)||defined(is_bar_c)
#define F_OK 0
#ifdef __cplusplus
extern "C" {
#endif
int close(int);
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
