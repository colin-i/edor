
#include "inc/stddef.h"

#ifdef __cplusplus
extern "C" {
#endif

void free(void*);
void*realloc(void*,size_t);
void*malloc(size_t);

char*getenv(const char*);

#ifdef __cplusplus
}
#endif