
#undef platform_is_known
#ifdef SIZEOF_INT_P
#	if SIZEOF_INT_P >= 8
#		ifdef SIZEOF_LONG
#			if SIZEOF_LONG == 4
#				define platform_is_known 1
				typedef long long time_t; //win 64 gcc
#			endif
#		endif
#	endif
#endif
#ifndef platform_is_known
	typedef long int time_t;
#else
#	undef platform_is_known
#endif

#ifdef __cplusplus
extern "C" {
#endif

time_t time(time_t*);

#ifdef __cplusplus
}
#endif
