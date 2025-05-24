
#undef platform_is_known
#ifdef SIZEOF_INT_P
#	if SIZEOF_INT_P >= 4
#		ifdef SIZEOF_LONG
#			define platform_is_known 1
#			if SIZEOF_LONG >= 4
				typedef long unsigned int size_t;
#			else
				typedef long long unsigned int size_t; //long long for windows
#			endif
#		endif
#	endif
#endif
#ifndef platform_is_known
	typedef unsigned int size_t;
#else
#	undef platform_is_known
#endif
