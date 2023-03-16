
#include "inc/stddef.h"

#define F_OK 0

typedef long int ssize_t;

#ifdef __cplusplus
extern "C" {
#endif

int access(const char*,int);
int close(int);
ssize_t write(int,const void*,size_t);

#ifdef __cplusplus
}
#endif
