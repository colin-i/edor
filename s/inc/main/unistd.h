
#include"inc/stddef.h"
//sys/types.h
typedef long off_t;
typedef int ssize_t;

#ifdef __cplusplus
extern "C" {
#endif

off_t lseek(int,off_t,int);
ssize_t read(int,void*,size_t);

int close(int);
ssize_t write(int,const void*,size_t);

#ifdef __cplusplus
}
#endif
