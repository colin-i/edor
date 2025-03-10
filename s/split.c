
#define is_split_c
#ifdef HAVE_STDDEF_H
#include<stddef.h>
#else
#include"inc/stddef.h"
#endif

#include "top.h" //bool,nullptr,etc

#ifdef HAVE_STDIO_H
#include<stdio.h>
#else
#include"inc/stdio.h"
#endif
#ifdef HAVE_STDLIB_H
#include<stdlib.h>
#else
#include"inc/stdlib.h"
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

#include "def.h"

bool splits_flag=false;

char*sdelimiter=(char*)"|||";
char*esdelimiter=(char*)"//";
char* sdelimiter_new=nullptr;
char* esdelimiter_new=nullptr;

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
		char a=*sdelimiter;
		unsigned char sdelimsize=strlen(sdelimiter);//at file read only one byte for size
		unsigned char esdelimsize=strlen(esdelimiter);//same from preferences

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
					calculated_new_size+=dif+(2*esdelimsize)+(2*ln_term_sz)+sz;
				}while(true);
				calculated_new_size+=size;

				char* newtext=(char*)malloc(calculated_new_size);
				if(newtext==nullptr){filesfree(files);return false;}

				//substitutions
				text=*p_text;size=*p_size;explodes=0;
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
					memcpy(cursor,esdelimiter,esdelimsize);cursor+=esdelimsize;
					//delim
					split_add(&text,next,&cursor,&size);

					//file
					next=(char*)memchr(text,a,size);
					split_add(&text,next,&cursor,&size);

					//ln_term
					memcpy(cursor,ln_term,ln_term_sz);cursor+=ln_term_sz;

					//content
					int f=files[explodes].file;
					lseek(f,0,(SEEK_SET));//was at size tell
					read(f,cursor,files[explodes].size);//can close now but is extra code
					cursor+=files[explodes].size;
					explodes++;

					//ln_term
					memcpy(cursor,ln_term,ln_term_sz);cursor+=ln_term_sz;

					//escape
					memcpy(cursor,esdelimiter,esdelimsize);cursor+=esdelimsize;
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

void split_writeprefs(int f){
	unsigned char sz=strlen(sdelimiter);
	if(write(f,&sz,extlen_size)==extlen_size){
		if(write(f,sdelimiter,sz)==sz){
			sz=strlen(esdelimiter);
			if(write(f,&sz,extlen_size)==extlen_size){
				#pragma GCC diagnostic push
				#pragma GCC diagnostic ignored "-Wunused-result"
				write(f,esdelimiter,sz);
				#pragma GCC diagnostic pop
			}
		}
	}
}
void split_readprefs(int f){
	unsigned char len;
	if(read(f,&len,extlen_size)==extlen_size){
		sdelimiter_new=(char*)malloc(len+1);
		if(sdelimiter_new!=nullptr){
			if(read(f,sdelimiter_new,len)==len){
				sdelimiter_new[len]='\0';
				sdelimiter=sdelimiter_new;
				if(read(f,&len,extlen_size)==extlen_size){
					esdelimiter_new=(char*)malloc(len+1);
					if(esdelimiter_new!=nullptr){
						if(read(f,esdelimiter_new,len)==len){
							esdelimiter_new[len]='\0';
							esdelimiter=esdelimiter_new;
						}
					}
				}
			}
		}
	}
}
void split_freeprefs(){
	if(sdelimiter_new!=nullptr){
		free(sdelimiter_new);
		if(esdelimiter_new!=nullptr){
			free(esdelimiter_new);
		}
	}
}
