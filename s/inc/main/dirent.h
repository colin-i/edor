
typedef struct __dirstream DIR;

#undef platform_is_known
#ifdef SIZEOF_INT_P
#	if SIZEOF_INT_P >= 8
#		ifdef SIZEOF_LONG
#			if SIZEOF_LONG == 4
#				define platform_is_known 1

struct dirent
{
 long d_ino;
 unsigned short d_reclen;
 unsigned short d_namlen;
 char d_name[260];
};

#			endif
#		endif
#	endif
#endif
#ifndef platform_is_known

#include "inc/types.h"
struct dirent
{
	__ino_t d_ino;
	__off_t d_off;
	unsigned short int d_reclen;
	unsigned char d_type;
	char d_name[256];
};
//same on pi 32bit

#endif

#ifdef __cplusplus
extern "C" {
#endif

int closedir(DIR *dirp);
DIR *opendir(const char *name);
struct dirent *readdir(DIR *dirp);

#ifdef __cplusplus
}
#endif
