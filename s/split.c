
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
// |||file.as||| to //|||file.as[ln_term]file content[ln_term]//|||

bool split_grab(char**p_text,size_t*p_size){
	if(splits_flag/*true*/){
		char*text;size_t size;
		const char*sdelimiter="|||";
		char a=*sdelimiter;

		//calculate number of explodes
		size_t explodes=0;
		text=*p_text;size=*p_size;
		do{
			char*next=(char*)memchr(text,a,size);
			if(next==nullptr)break;
			next++;
			size-=next-text;
			text=next;
			explodes++;
		}while(true);
		if(explodes!=0){
			//alloc files[explodes],sizes[explodes]

			//final size
			size_t calculated_new_size=0;
			text=*p_text;size=*p_size;
			do{
				char*next=(char*)memchr(text,a,size);
				if(next==nullptr)break;
				next++;
				size_t dif=next-text;
				calculated_new_size+=dif;//+2ln_term_sz +2escape_delimiter
				size-=dif;
				text=next;
			}while(true);
			calculated_new_size+=size;

			char*newmem=(char*)realloc(*p_text,calculated_new_size);
			if(newmem!=nullptr){
				__builtin_trap();
				return true;
			}
			return false;
		}
	}
	return true;
}
