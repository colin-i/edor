
#ifdef __cplusplus
extern "C"{
#endif

//this is fiction, HAVE_ERRNO_LOCATION can be defined if no HAVE_ERRNO_H, is good only for fast compilers (make test)
#ifdef HAVE_ERRNO_LOCATION
	extern int *__errno_location (void) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));
#	define errno (*__errno_location ())
#else
	int* __errno(void) __attribute__((__const__));
#	define errno (*__errno())
#endif

#ifdef __cplusplus
}
#endif
