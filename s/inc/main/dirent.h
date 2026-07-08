
#include "inc/types.h"

typedef struct __dirstream DIR;

#define DT_DIR 4
#define DT_LNK 10

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

int dirfd(DIR *dirp);
DIR *fdopendir(int fd);
struct dirent *readdir(DIR *dirp);
int closedir(DIR *dirp);

#ifdef __cplusplus
}
#endif
