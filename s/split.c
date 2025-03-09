
#include "top.h" //bool,nullptr,etc

#ifdef HAVE_STDLIB_H
#include<stdlib.h>
#else
#include"inc/split/stdlib.h"
#endif
#ifdef HAVE_STDIO_H
#include<stdio.h>
#else
#include"inc/stdio.h"
#endif
#ifdef HAVE_STRING_H
#include<string.h>
#else
#include "inc/split/string.h"
#endif
#ifdef HAVE_UNISTD_H
#include<unistd.h>
#else
#include"inc/unistd.h"
#endif
#ifdef HAVE_FCNTL_H
#include<fcntl.h>
#else
#include"inc/fcntl.h"
#endif

bool splits_flag=true;
// |||file.as||| to //|||file.as[ln_term]file content[ln_term]//|||

typedef struct{
	int file;
	off_t size;
}filedata;

static void filesfree(filedata*files){
	for(size_t i=0;files[i].file!=-1;i++)close(files[i].file);
	free(files);
}
static void split_error(){
	puts("Splits are activated and the syntax is wrong or a split file can't be taken. Disable splits (open without a file and follow the help).");
}
static void split_error_free(filedata*files){
	filesfree(files);
	split_error();
}
static void split_add(char**text,char*next,char**newtext,size_t*remaining_size){
	size_t dif=next-*text;
	memcpy(*newtext,*text,dif);
	*text+=dif;*newtext+=dif;*remaining_size-=dif;
}

bool split_grab(char**p_text,size_t*p_size){
	if(splits_flag/*true*/){
		char*text;size_t size;int cmp;char*next;
		char*sdelimiter="|||";
		char a=*sdelimiter;
		size_t sdelimsize=strlen(sdelimiter);//size_t? this can be from file read

		//calculate number of explodes
		size_t explodes=0;
		text=*p_text;size=*p_size;
		do{
			next=(char*)memchr(text,a,size);
			if(next==nullptr)break;
			next++;size-=next-text;
			if(size<=(sdelimsize-1))break;
			cmp=memcmp(next,sdelimiter+1,sdelimsize-1);
			next+=sdelimsize-1;text=next;size-=next-text;
			if(cmp!=0)continue;

			next=(char*)memchr(text,a,size);
			if(next==nullptr){split_error();return false;}
			next++;size-=next-text;
			if(size<=(sdelimsize-1)){split_error();return false;}
			cmp=memcmp(next,sdelimiter+1,sdelimsize-1);
			if(cmp!=0){split_error();return false;}//it looks like the delimiter chars can't be in filename chars
			next+=sdelimsize-1;text=next;size-=next-text;

			explodes++;
		}while(true);
		if(explodes!=0){
			filedata*files=(filedata*)malloc(sizeof(filedata)*(explodes+1));//+1 because decided to not global store number of explodes
			if(files!=nullptr){
				files[0].file=-1;
				explodes=0;

				//final size
				size_t calculated_new_size=0;
				text=*p_text;size=*p_size;
				do{
					size_t dif;

					next=(char*)memchr(text,a,size);
					if(next==nullptr)break;
					next++;dif=next-text;size-=dif;calculated_new_size+=dif;
					if(size<=(sdelimsize-1))break;
					cmp=memcmp(next,sdelimiter+1,sdelimsize-1);
					next+=sdelimsize-1;text=next;dif=next-text;size-=dif;calculated_new_size+=dif;
					if(cmp!=0)continue;

					next=(char*)memchr(text,a,size);
					*next='\0';
					int f=open(text,O_RDONLY);
					if(f==-1){split_error_free(files);return false;}
					files[explodes].file=f;
					off_t sz=lseek(f,0,(SEEK_END));
					if(sz==-1){split_error_free(files);return false;}
					files[explodes].size=sz;
					explodes++;
					files[explodes].file=-1;
					*next=a;

					next+=sdelimsize;text=next;dif=next-text;size-=dif;
					calculated_new_size+=dif;//+2ln_term_sz +2escape_delimiter +sz
				}while(true);
				calculated_new_size+=size;

				char* newtext=(char*)malloc(calculated_new_size);
				if(newtext==nullptr){filesfree(files);return false;}

				//substitutions
				text=*p_text;size=*p_size;
				char*cursor=newtext;
				do{
					next=(char*)memchr(text,a,size);
					if(next==nullptr)break;
					split_add(&text,next,&cursor,&size);

					if(size<=sdelimsize)break;
					cmp=memcmp(next+1,sdelimiter+1,sdelimsize-1);
					next+=sdelimsize;
					if(cmp!=0){
						split_add(&text,next,&cursor,&size);
						continue;
					}

					//escape
					//delim
					split_add(&text,next,&cursor,&size);

					//file
					next=(char*)memchr(text,a,size);
					split_add(&text,next,&cursor,&size);

					//ln_term

					//content
					//lseek(f,0,(SEEK_SET));

					//ln_term

					//escape
					//delim
					next+=sdelimsize;
					split_add(&text,next,&cursor,&size);
				}while(true);
				memcpy(cursor,text,size);

				free(*p_text);
				*p_text=newtext;*p_size=calculated_new_size;
				filesfree(files);
				return true;
			}
			return false;
		}
	}
	return true;
}
