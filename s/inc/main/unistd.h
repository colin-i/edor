
#define STDIN_FILENO 0

#define SEEK_CUR 1

#include "inc/unistd.h"

#ifdef __cplusplus
extern "C" {
#endif

int ftruncate(int,off_t);
int isatty(int);
int rmdir(const char *);
int unlink(const char*);

#ifdef __cplusplus
}
#endif
