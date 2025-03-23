
#ifdef __cplusplus
extern "C"{
#endif

//int* __errno(void) __attribute__((__const__));
//#define errno (*__errno())
extern int *__errno_location (void) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));
#define errno (*__errno_location ())

#ifdef __cplusplus
}
#endif
