
#define SEEK_CUR 1

#include "inc/unistd.h"

#ifdef __cplusplus
extern "C" {
#endif

int ftruncate(int,off_t);
int rmdir(const char *);
int unlink(const char*);

#ifdef __cplusplus
}
#endif
