
#include "inc/stdio.h"

typedef struct __sFILE FILE;
//extern FILE*stdin __attribute__((annotate("introduced_in=" "23")));//warning on headless
extern FILE *stdin;//this is amd def

#ifdef __cplusplus
extern "C" {
#endif

FILE* freopen(const char *filename, const char *mode, FILE *stream);
int getchar(void);
int printf(const char *, ...);
int putchar(int);
int rename(const char*,const char*);
//int snprintf(char* __buf, size_t __size, const char* __fmt, ...);

#ifdef __cplusplus
}
#endif
