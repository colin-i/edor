
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
char*esdelimiter=(char*)"///";
char* sdelimiter_new=nullptr;
char* esdelimiter_new=nullptr;
char*split_out=(char*)"";
char*split_out_new=nullptr;

//char*fulldelim;
//unsigned short fulldelim_size;
unsigned char sdelimiter_size;
unsigned char esdelimiter_size;

static int split_out_file;

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
			text=next+(sdelimsize-1);size-=text-next;
			if(cmp!=0)continue;

			next=(char*)memchr(text,a,size);
			if(next==nullptr){split_error();return false;}
			next++;size-=next-text;
			if(size<=(sdelimsize-1)){split_error();return false;}
			cmp=memcmp(next,sdelimiter+1,sdelimsize-1);
			if(cmp!=0){split_error();return false;}//it looks like the delimiter chars can't be in filename chars
			text=next+(sdelimsize-1);size-=text-next;

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
					text=next+(sdelimsize-1);dif=text-next;size-=dif;calculated_new_size+=dif;
					if(cmp!=0)continue;
					calculated_new_size-=sdelimsize;//we are using another marker at view

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

					dif=next-text;
					text=next+sdelimsize;
					size-=dif+sdelimsize;
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
					//split_add(&text,next,&cursor,&size);
					size-=next-text;text=next;

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
					//split_add(&text,next,&cursor,&size);
					size-=next-text;text=next;
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
				if(write(f,esdelimiter,sz)==sz){
					sz=strlen(split_out);
					if(write(f,&sz,extlen_size)==extlen_size){
						#pragma GCC diagnostic push
						#pragma GCC diagnostic ignored "-Wunused-result"
						write(f,split_out,sz);
						#pragma GCC diagnostic pop
					}
				}
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
							if(read(f,&len,extlen_size)==extlen_size){
								split_out_new=(char*)malloc(len+1);
								if(split_out_new!=nullptr){
									if(read(f,split_out_new,len)==len){
										split_out_new[len]='\0';
										split_out=split_out_new;
									}
								}
							}
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
			if(split_out_new!=nullptr){
				free(split_out_new);
			}
		}
	}
}

//true at ok
bool split_write_init(char*orig_filename){
	sdelimiter_size=strlen(sdelimiter);
	esdelimiter_size=strlen(esdelimiter);
	//unsigned char s2=strlen(esdelimiter);fulldelim_size=sdelimiter_size+s2;fulldelim=(char*)malloc(fulldelim_size);if(fulldelim!=nullptr){memcpy(fulldelim,esdelimiter,s2);memcpy(fulldelim+s2,sdelimiter,sdelimiter_size);
	if(*split_out!='\0'){
		size_t a=strlen(split_out);
		size_t b=a+1;
		size_t c=strlen(orig_filename);
		char*d=(char*)malloc(b+c+1);
		if(d!=nullptr){
			memcpy(d,split_out,a);
			d[a]=path_separator;
			memcpy(d+b,orig_filename,c);
			d[b+c]='\0';
			split_out_file=open_or_new(d);
			free(d);
			if(split_out_file!=-1){
				return true;
			}
		}
		return false;
	}
	return true;
}
void split_write_free(){
	//free(fulldelim);
	if(*split_out!='\0'){
		close(split_out_file);
	}
}

bool swrite(int f,void*buf,unsigned int size){
	if(write(f,buf,size)==size){
		if(*split_out!='\0'){
			if(write(split_out_file,buf,size)==size){
				return swrite_ok;
			}
			return swrite_bad;
		}
		return swrite_ok;
	}
	return swrite_bad;
}
//true if ok
static bool split_write_split(char*file,size_t start,size_t end,bool*majorerror){
	int f=open_or_new(file);
	if(f!=-1){
		for(size_t k=start;k<end;k++){
			row*r=&rows[k];
			unsigned int sz=r->sz;
			if(swrite(f,r->data,sz)/*true*/){close(f);*majorerror=true;return false;}
			if(swrite(f,ln_term,ln_term_sz)/*true*/){close(f);*majorerror=true;return false;}
		}
		row*r=&rows[end];unsigned int sz=r->sz;
		if(swrite(f,r->data,sz)==swrite_ok){close(f);return true;}
		close(f);*majorerror=true;return false;
	}
	clue=start;*majorerror=false;return false;
}
static bool split_write_orig(int orig_file,char*cursor,unsigned int size,bool*majorerror){
	if(write(orig_file,sdelimiter,sdelimiter_size)==sdelimiter_size)
		if(write(orig_file,cursor,size)==size)
			if(write(orig_file,sdelimiter,sdelimiter_size)==sdelimiter_size)
				return true;
	*majorerror=true;return false;
}
//true if the row has split start syntax and a split end syntax exists
const char* split_write(size_t*_index,int orig_file,unsigned int*_off,bool*majorerror){
	size_t i=*_index;
	row*rw=&rows[i];
	char*data=rw->data+*_off;
	unsigned int size=rw->sz-*_off;
	char*pointer=(char*)memmem(data,size,esdelimiter,esdelimiter_size);//fulldelim,fulldelim_size
	if(pointer!=nullptr){
		char*cursor=pointer+esdelimiter_size;//fulldelim_size
		size-=cursor-data;
		if(size!=0){
			i++;
			for(size_t j=i+1;j<rows_tot;j++){//+1, if blank there is the empty row
				if(memcmp((&rows[j])->data,esdelimiter,esdelimiter_size)==0){//fulldelim,fulldelim_size
					*_index=j;*_off=esdelimiter_size;//fulldelim_size

					if(swrite(orig_file,data,pointer-data)==swrite_ok){
						//char aux=cursor[size];//also alloced rows have +1
						cursor[size]='\0';//this is for unmodified where ln_term is there, for alloced is undefined there
						//cursor[size]=aux;//is not important to have ln_term back there
						if(split_write_split(cursor,i,j-1,majorerror)/*true*/)
							{if(split_write_orig(orig_file,cursor,size,majorerror)/*true*/)return nullptr;}
						else if(*majorerror==false)split_write_orig(orig_file,cursor,size,majorerror);
					}else *majorerror=true;
					return "Error(s) at split write";
				}
			}
		}
	}
	return nullptr;
}
