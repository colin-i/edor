
#define is_split_c
#ifdef HAVE_STDDEF_H
#include<stddef.h>
#else
#include"inc/stddef.h"
#endif

#include "top.h" //bool,nullptr,etc

#ifdef HAVE_FCNTL_H
#include<fcntl.h>
#else
#include"inc/fcntl.h"
#endif
#ifdef HAVE_ERRNO_H
#include<errno.h>
#else
#include"inc/split/errno.h"
#endif
#ifdef HAVE_STDIO_H
#include<stdio.h>
#else
#include"inc/stdio.h"
#endif
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
#ifdef HAVE_UNISTD_H
#include<unistd.h>
#else
#include"inc/split/unistd.h"
#endif

#include "def.h"

bool splits_flag=false;
split_char split_read_atstart=split_no;

char*sdelimiter=(char*)"|||";// iso forbids
char*esdelimiter=(char*)"///";
char* sdelimiter_new=nullptr;
char* esdelimiter_new=nullptr;
char*split_out=(char*)"osrc";
char*split_out_new=nullptr;
char*split_extension=default_extension;
char*split_extension_new=nullptr;

//char*fulldelim;
//unsigned short fulldelim_size;
static unsigned char sdelimiter_size;
static unsigned char esdelimiter_size;

static char*split_out_alloc1;
static char*split_out_alloc2;
static char*split_out_path1;
static char*split_out_path2;
static char*split_out_path3;
static char*split_out_path4;
static unsigned char split_out_size1;
static size_t split_out_size2;

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

static char* real_path(char*pathname){
	char*a=realpath(pathname,nullptr);
	if(a==nullptr&&errno==ENOENT){//is a New Path
		size_t sz=strlen(pathname);
		char*b=pathname+sz;
		while(pathname!=b){
			b--;
			if(*b==path_separator){
				*b='\0';
				a=realpath(pathname,nullptr);
				*b=path_separator;
				if(a!=nullptr){
					b++;
					break;
				}else if(errno!=ENOENT)return nullptr;
			}
		}
		if(a==nullptr){
			a=realpath(".",nullptr);//realpath "" is (null)
			if(a==nullptr)return nullptr;
		}
		size_t left=strlen(a);
		size_t left_and_center=left+1;
		size_t right=strlen(b)+1;
		char*c=(char*)realloc(a,left_and_center+right);
		if(c!=nullptr){
			c[left]=path_separator;
			memcpy(c+left_and_center,b,right);
			return c;
		}
		free(a);
		return nullptr;
	}
	return a;
}
//-1 no, 0 errors, 1 yes the split_out is at realpath dirname or in an ancestor folder
static split_char split_conditions_out(char*filename,bool free_paths){
	if(*split_out!='\0'){
		split_out_alloc1=real_path(filename);
		if(split_out_alloc1!=nullptr){
			size_t s=strlen(split_out_alloc1);
			split_out_path1=split_out_alloc1+s;
			split_out_path2=split_out_path1;
			while(split_out_alloc1!=split_out_path2){
				split_out_path2--;if(*split_out_path2==path_separator){
					split_out_size1=strlen(split_out)+1;
					split_out_path3=split_out_path2+1;
					size_t size=split_out_path3-split_out_alloc1;
					split_out_size2=size+split_out_size1;
					split_out_alloc2=(char*)malloc(split_out_size2);//this at maximum but can be smaller if two ancestors until match
					if(split_out_alloc2!=nullptr){
						memcpy(split_out_alloc2,split_out_alloc1,size);
						char*d=split_out_alloc2-1;//also, look in root
						split_out_path4=split_out_alloc2+(split_out_path2-split_out_alloc1);
						split_out_path2=split_out_path4;//this is for split write to not loop again there
						while(d!=split_out_path4){
							if(*split_out_path4==path_separator){
								memcpy(split_out_path4+1,split_out,split_out_size1);
								if(access(split_out_alloc2,W_OK)==0){
									//special cases with bad overwrites:
									//	1. split out is the same folder: out and main file will collide. change split out folder
									//	2. split out is another folder: out and a split can collide. change the split location
									//	3. a split is same as main file will be a collision. change the split location
									//	4. two same splits will be a collision
									//		can be tolerated if one is equal or included. else change the split location
									//anyway, who will wrong these? don't write extra code for the moment
									if(free_paths/*true*/){
										free(split_out_alloc1);
										free(split_out_alloc2);
									}
									return split_yes;
								}
								split_out_path2=split_out_path4;//this is for split write to not loop again there
							}
							split_out_path4--;
						}
						free(split_out_alloc1);free(split_out_alloc2);
						return split_no;
					}
					free(split_out_alloc1);
					return split_err;
				}
			}
			free(split_out_alloc1);
		}else if(errno==EACCES)return split_no;
		return split_err;
	}
	return split_no;
}
split_char split_conditions(char*filename,bool free_paths){
	if(splits_flag/*true*/){
		if(is_extension_ok(split_extension,filename)/*true*/){
			return split_conditions_out(filename,free_paths);
		}
	}
	return split_no;
}

//false on errors
bool split_grab(char**p_text,size_t*p_size,char*argfile){
	split_read_atstart=split_conditions(argfile,true);
	if(split_read_atstart!=split_err){
		if(split_read_atstart==split_yes){
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
				if(size<(sdelimsize-1)){split_error();return false;} //can be equal and is last char of the file
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
					unsigned short calculated_fixed=(2*esdelimsize)+ln_term_sz-sdelimsize;
					do{
						size_t dif;

						next=(char*)memchr(text,a,size);
						if(next==nullptr)break;
						next++;dif=next-text;size-=dif;calculated_new_size+=dif;
						if(size<=(sdelimsize-1))break;
						cmp=memcmp(next,sdelimiter+1,sdelimsize-1);
						text=next+(sdelimsize-1);dif=text-next;size-=dif;calculated_new_size+=dif;
						if(cmp!=0)continue;
						//calculated_new_size-=sdelimsize;//we are using another marker at view

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
						calculated_new_size+=dif+sz+calculated_fixed;
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
						#pragma GCC diagnostic push
						#pragma GCC diagnostic ignored "-Wunused-result"
						read(f,cursor,files[explodes].size);//can close now but is extra code
						#pragma GCC diagnostic pop
						cursor+=files[explodes].size;
						explodes++;

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
	return false;
}

bool split_writeprefs(int f){
	unsigned char sz=strlen(sdelimiter);
	if(write(f,&sz,extlen_size)==extlen_size){
		if(write(f,sdelimiter,sz)==sz){
			sz=strlen(esdelimiter);
			if(write(f,&sz,extlen_size)==extlen_size){
				if(write(f,esdelimiter,sz)==sz){
					sz=strlen(split_out);
					if(write(f,&sz,extlen_size)==extlen_size){
						if(write(f,split_out,sz)==sz){
							sz=strlen(split_extension);
							if(write(f,&sz,extlen_size)==extlen_size){
								if(write(f,split_extension,sz)==sz){
									return true;
								}
							}
						}
					}
				}
			}
		}
	}
	return false;
}
bool split_readprefs(int f){
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
										if(read(f,&len,extlen_size)==extlen_size){
											split_extension_new=(char*)malloc(len+1);
											if(split_extension_new!=nullptr){
												if(read(f,split_extension_new,len)==len){
													split_extension_new[len]='\0';
													split_extension=split_extension_new;
													return true;
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
		}
	}
	return false;
}
void split_freeprefs(){
	if(sdelimiter_new!=nullptr){
		free(sdelimiter_new);
		if(esdelimiter_new!=nullptr){
			free(esdelimiter_new);
			if(split_out_new!=nullptr){
				free(split_out_new);
				if(split_extension_new!=nullptr){
					free(split_extension_new);
				}
			}
		}
	}
}

//true at ok
bool split_write_init(char*orig_filename){
	sdelimiter_size=strlen(sdelimiter);
	esdelimiter_size=strlen(esdelimiter);
	//unsigned char s2=strlen(esdelimiter);fulldelim_size=sdelimiter_size+s2;fulldelim=(char*)malloc(fulldelim_size);if(fulldelim!=nullptr){memcpy(fulldelim,esdelimiter,s2);memcpy(fulldelim+s2,sdelimiter,sdelimiter_size);

	//if(*split_out!='\0'){//this can't be, here is after a split_yes that is only at end of split_conditions_out, where this is
	size_t ancestors_diff=split_out_path2-split_out_path4;
	split_out_path2=split_out_alloc1+(split_out_path2-split_out_alloc2);
	split_out_path4+=split_out_size1;
	do{
		*split_out_path4=path_separator;
		split_out_path2++;split_out_path4++;
		if(split_out_path2==split_out_path3)break;
		char*a=split_out_path2;
		do{a++;}while(*a!=path_separator);
		size_t sz=a-split_out_path2;
		memcpy(split_out_path4,split_out_path2,sz);
		split_out_path2+=sz;split_out_path4+=sz;
	}while(true);
	size_t size=split_out_path1-split_out_path3+1;
	split_out_size2-=ancestors_diff;
	char*a=(char*)realloc(split_out_alloc2,split_out_size2+size);
	if(a!=nullptr){
		memcpy(a+split_out_size2,split_out_path3,size);
		free(split_out_alloc1);
		split_out_file=open_or_new(a);
		free(a);
		if(split_out_file!=-1){
			return true;
		}
		return false;
	}
	free(split_out_alloc1);free(split_out_alloc2);
	return false;
	//}//return true;
}
void split_write_free(){
	//free(fulldelim);
	//if(*split_out!='\0'){//this can't be. is only after a successful split_write_init
	close(split_out_file);
	//}
}

swrite_char swrite(int f,void*buf,row_dword size){
	if(write(f,buf,size)==size){
		//if(*split_out!='\0'){//this can't be, all swrites are in split way
		if(write(split_out_file,buf,size)==size){
			return swrite_ok;
		}
		//return swrite_bad;
		//}//return swrite_ok;
	}
	return swrite_bad;
}
//swrite_char swwrite(int f,void*buf,row_dword size){
//	if(filewhites_bool/*true*/){
//		char*b=(char*)buf;
//		char*last=b+size;
//		while(b!=last&&*b!='\t')b++;//0 size is ok
//		if(b!=buf){
//			rowd_dword i=b-buf;
//			size-=i;
//			char w=' ';
//			while(i!=0){swrite(f,&w,1);i--;}
//			buf=b;
//		}
//	}
//	return swrite(f,buf,size);
//}

//true if ok
static bool split_write_split(char*file,size_t start,size_t end,unsigned int size,bool*majorerror){
	int f=open_or_new(file);
	if(f!=-1){
		for(size_t k=start;k<end;k++){
			row*r=&rows[k];
			unsigned int sz=r->sz;
			if(swrite(f,r->data,sz)/*true*/){close(f);*majorerror=true;return false;}
			if(swrite(f,ln_term,ln_term_sz)/*true*/){close(f);*majorerror=true;return false;}
		}
		row*r=&rows[end];
		if(swrite(f,r->data,size)==swrite_ok){close(f);return true;}
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
//null or error
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
			for(size_t j=i;j<rows_tot;j++){
				rw=&rows[j];
				char*content=rw->data;
				unsigned int sz=rw->sz;
				char*marker=(char*)memmem(content,sz,esdelimiter,esdelimiter_size);//fulldelim,fulldelim_size
				if(marker!=nullptr){
					unsigned int part=marker-content;
					*_index=j;*_off=part+esdelimiter_size;//fulldelim_size

					if(swrite(orig_file,data,pointer-data)==swrite_ok){
						//char aux=cursor[size];//also alloced rows have +1
						cursor[size]='\0';//this is for unmodified where ln_term is there, for alloced is undefined there
						//cursor[size]=aux;//is not important to have ln_term back there
						if(split_write_split(cursor,i,j,part,majorerror)/*true*/)
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
