
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

#define bar_byte unsigned char
#define bar_byte_plus short
#define row_dword unsigned int

typedef struct{
	char*data;
	row_dword spc;//at row_alloc(type,undo,...),pasting,enter, must be +1 for curses addstr and at split where right part is the file name
	row_dword sz;//at memtrm to work more to avoid 64 bit rows?
}row;
//typedef struct {}rowwrap;

typedef struct{
	char**orig;
	char**buf;
	bool sizedonly;
}extdata;

#define Char_Return 0xd  //main bar tit
#define Char_Escape 0x1b //main
#define Char_Ctrl 0x5e   //main bar tit

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
#define com_nr_swkey 6
	#define com_nr_passcursor_numbers com_nr_swkey
#define com_nr_save 7
#define com_nr_ext 8

#define is_word_char(a) ('0'<=a&&(a<='9'||('A'<=a&&(a<='Z'||(a=='_'||('a'<=a&&a<='z'))))))

#define max_path 0xff
#define max_path_0 max_path+1

#define command_resize -2
#define command_no -1 //(char)-1 but there is at least a r> comparation
#define command_false 0
#define command_ok 1
#define command_char signed char

#define movement_processed -1
#define movement_diffkey 0
#define movement_resize 1
#define movement_char signed char

#define split_no -1
#define split_err 0
#define split_yes_mixless 1
#define split_yes_mix 2
#define split_char signed char

#define comnrp_define char*

#define color_0 0
#define color_a 1
#define color_b 2
#define color_c 3
#define color_d 4

#define extlen_size 1

#define swrite_ok false
#define swrite_bad true
#define swrite_char bool

#define protocol_simple "%u"
#ifdef PLATFORM64
#define protocol "%lu"
#else
#define protocol protocol_simple
#endif

//main,bar
typedef struct{
	char* key_location;
	unsigned short*pos;
	char pos_total;
	unsigned short upos;
	char index;
}key_struct;
