
#undef platform_is_known
#ifdef SIZEOF_INT_P
#	if SIZEOF_INT_P >= 8
#		ifdef SIZEOF_LONG
#			define platform_is_known 1
#			if SIZEOF_LONG == 4
				typedef long long unsigned int size_t; //long long for windows
#			else
				typedef long unsigned int size_t;
#			endif
#		endif
#	endif
#endif
#ifndef platform_is_known
	typedef unsigned int size_t;
#else
#	undef platform_is_known
#endif
