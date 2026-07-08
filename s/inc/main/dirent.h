
#include "inc/types.h"

typedef struct __dirstream DIR;

struct dirent
{
	__ino_t d_ino;
	__off_t d_off;

	unsigned short int d_reclen;
	unsigned char d_type;
	char d_name[256];
};

#ifdef __cplusplus
extern "C" {
#endif

DIR *fdopendir(int fd);
struct dirent *readdir(DIR *dirp);
int closedir(DIR *dirp);

#ifdef __cplusplus
}
#endif
