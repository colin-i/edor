
#include "sep.h"

#ifndef __cplusplus
#define nullptr 0
#endif

#if (defined(USE_FS) || defined(USE__FS))
#include<filesystem>
#ifdef USE_FS
char path_separator=std::filesystem::path::preferred_separator;
#else
char path_separator=std::__fs::filesystem::path::preferred_separator;
#endif

#else
char path_separator;
#ifdef HAVE_STDLIB_H
#include"stdlib.h"
#else
#include"inc/sep/stdlib.h"
#endif
#ifdef HAVE_STRING_H
#include"string.h"
#else
#include"inc/sep/string.h"
#endif
void set_path_separator(char*s){
	size_t i=strlen(s);
	do{
		i--;
		if(s[i]=='/'){
			path_separator='/';
			return;
		}
		if(s[i]=='\\'){
			path_separator='\\';
			return;
		}
	}while(i);
	s=getenv("HOME");
	if(s!=nullptr){
		i=strlen(s);
		if(strrchr(s,'/')!=nullptr){
			path_separator='/';
			return;
		}
		else if(strrchr(s,'\\')!=nullptr){
			path_separator='\\';
			return;
		}
	}
	path_separator='\\';
}
#endif
