
#include "top.h" //bool,nullptr,etc

#ifdef HAVE_STDLIB_H
#include<stdlib.h>
#else
#include"inc/split/stdlib.h"
#endif
#ifdef HAVE_STRING_H
#include<string.h>
#else
#include "inc/split/string.h"
#endif

bool splits_flag=true;

bool split_grab(char**p_text,size_t*p_size){
	if(splits_flag/*true*/){
		char*text=*p_text;size_t size=*p_size;
		size_t calculated_new_size=0;
		const char*sdelimiter="|||";
		char a=*sdelimiter;
		do{
			char*next=(char*)memchr(text,a,size);
			if(next==nullptr)break;
			next++;
			size_t dif=next-text;
			calculated_new_size+=dif;
			size-=dif;
			text=next;
		}while(true);
		calculated_new_size+=size;
		if(*p_size!=calculated_new_size){
			char*newmem=(char*)realloc(*p_text,calculated_new_size);
			if(newmem!=nullptr){
				//__builtin_trap();
				return true;
			}
			return false;
		}
	}
	return true;
}
