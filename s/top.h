
#ifndef __cplusplus

#ifdef HAVE_STDBOOL_H
#include<stdbool.h>
#else
//
#ifdef _Bool
typedef typeof(_Bool) bool;
#else
typedef char bool;
#endif
//
enum{false=0!=0,true=1==1};
#endif

#include "null.h"
#endif

typedef struct{
	char*data;
	unsigned int spc;//at row_alloc(type,undo,...),pasting,enter, must be +1 for curses addstr and at split where right part is the file name
	unsigned int sz;
}row;
//typedef struct {}rowwrap;

#define Char_Return 0xd
#define row_pad 0xF
#define tab_sz 6
//can be 127(ascii Delete) or 263, note: Ctrl+h generates 263
#define is_KEY_BACKSPACE(a) a==KEY_BACKSPACE||a==0x7f

#define com_nr_find 0
#define com_nr_findagain 1
#define com_nr_findword 2
#define com_nr_findwordfrom 3
	#define com_nr_find_numbers com_nr_findwordfrom
#define com_nr_goto 4
#define com_nr_goto_alt 5
	#define com_nr_goto_numbers com_nr_goto_alt
#define com_nr_save 6
#define com_nr_ext 7

#define is_word_char(a) ('0'<=a&&(a<='9'||('A'<=a&&(a<='Z'||(a=='_'||('a'<=a&&a<='z'))))))

#define max_path 0xff
#define max_path_0 max_path+1

#define command_return_ok 1

#define comnrp_define char*

#define color_0 0
#define color_a 1
#define color_b 2
#define color_c 3
#define color_d 4

#define extlen_size 1
