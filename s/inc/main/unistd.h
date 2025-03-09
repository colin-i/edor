
#define SEEK_SET 0

#include "inc/unistd.h"

#ifdef __cplusplus
extern "C" {
#endif

ssize_t read(int,void*,size_t);
int rmdir(const char *);
int unlink(const char*);

#ifdef __cplusplus
}
#endif
