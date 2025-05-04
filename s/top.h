
#ifndef __cplusplus

#ifdef HAVE_STDBOOL_H
#include<stdbool.h>
#else
//
#ifdef _Bool
typedef typeof(_Bool) bool;
enum{false=0!=0,true=1==1};
//#else //bool is a keyword with -std=c23 onwards
//typedef char bool;
//enum{false... //false is a keyword with -std=c23 onwards
#endif
//
#endif

#include "null.h"
#endif


//these are going also in split

#define row_dword unsigned int

typedef struct{
	char*data;
	row_dword spc;//at row_alloc(type,undo,...),pasting,enter, must be +1 for curses addstr and at split where right part is the file name
	row_dword sz;//at memtrm to work more to avoid 64 bit rows?
}row;
//typedef struct {}rowwrap;

#define swrite_bad -1
#define swrite_ok 0
#define swrite_char signed char

#define extlen_size 1

#define split_no -1
#define split_err 0
#define split_yes_mixless 1
#define split_yes_mix 2
#define split_char signed char
