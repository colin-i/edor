

//can add //comment if a macro in a set is only for a file but don't spread them and make a mess

#include "def.h"

//main,bar
#ifdef _KEY_BACKSPACE
#	undef KEY_BACKSPACE
#	define KEY_BACKSPACE _KEY_BACKSPACE   //0x7f at termux and 0x8 at pdcurses
#endif
#define bar_byte_plus short
#define com_nr_find 0
#define com_nr_findagain 1
#define com_nr_findword 2
#define com_nr_findwordfrom 3
	#define com_nr_find_numbers com_nr_findwordfrom
#define com_nr_goto 4
#define com_nr_goto_alt 5
#define com_nr_swkey 6
#define com_nr_tab 7
#define com_nr_restore 8
	#define com_nr_passcursor_numbers com_nr_restore
#define com_nr_save 8
#define com_nr_ext 9
#define comnrp_define char*
#define command_resize -2
#define command_no -1 //(char)-1 but there is at least a r> comparation
#define command_false 0
#define command_ok 1
#define command_char signed char
typedef struct{
	char**orig;
	char**buf;
	bool sizedonly;
}extdata;
#define is_word_char(a) ('0'<=a&&(a<='9'||('A'<=a&&(a<='Z'||(a=='_'||('a'<=a&&a<='z'))))))
#define max_path 0xff
#define max_path_0 max_path+1

#define protocol_simple "%u"
#undef protocol
#ifdef SIZEOF_INT_P
#	if SIZEOF_INT_P >= 8
#		ifdef SIZEOF_LONG
#			if SIZEOF_LONG == 4
#				define protocol "%llu" //long long for windows
#			else
#				define protocol "%lu"
#			endif
#		endif
#	endif
#endif
#ifndef protocol
#	define protocol protocol_simple
#endif

#define row_pad 0xF
#define tab_conditions_low '2'
#define tab_conditions_high '9'
#define tab_conditions(a) a>=tab_conditions_low&&a<=tab_conditions_high
#define tab_conditions_nr(a) a>=(tab_conditions_low-'0')&&a<=(tab_conditions_high-'0')
typedef struct{
	char* key_location;
	unsigned short*pos;
	char pos_total;
	unsigned short upos;
	char index;
}key_struct;
typedef struct{
	char key;
	char add;
}show_key_struct;
#define default_timeout 60
//main,bar,tit
#define Char_Return 0xd  //main bar tit
#define Char_Escape 0x1b //main
#define Char_Ctrl 0x5e   //main bar tit
#define color_0 0
#define color_a 1
#define color_b 2
#define color_c 3
#define color_d 4
//main,tw,tit
#define movement_processed -1
#define movement_diffkey 0
#define movement_resize 1
#define movement_char signed char

//main
//bar
int c_to_xc(int,int);
void changekey(unsigned char);
void cpymembuf(size_t,row_dword,size_t,row_dword,char*);
void deleting(size_t,size_t,size_t,size_t);
bool deleting_init(size_t,row_dword,size_t,row_dword);
void editing_rebase();
void mod_set_off();
void mod_set_off_wrap();
void mod_set_on();
bool no_char(char);
void orig_key_show(show_key_struct*);
bool paste(size_t,row_dword,row_dword*,char*,size_t,size_t,bool);
void pref_modify(char**,char**,bool,char*,bar_byte);
void printinverted(const char*s);
void refreshrowsbot(WINDOW*,int,int);
#define refreshrows(w,i) refreshrowsbot(w,i,getmaxy(w))
void restore_rebase();
#define mask_nomask 0
#define ignored 0
#define rewriteprefs setprefs(mask_nomask,ignored)
bool row_alloc(row*,row_dword,size_t,row_dword);
void row_set(row*,row_dword,row_dword,row_dword,const char*);
void setprefs(int,bool);
size_t sizemembuf(size_t,row_dword,size_t,row_dword);
void vis(char,WINDOW*);
extern bool filewhites_flag;
extern char*filewhites_extension;
extern bool insensitive;
extern key_struct*keys;
extern char key_save;
extern char*keys_row;
extern char*keys_help;
extern bool mod_flag;
extern char*ocode_extension;
extern char*textfile;
extern unsigned short timeout_duration;
//bar,tw
extern char tab_sz;
//bar,tw,tit
void visual(char);
void fixed_yx(size_t*,row_dword*,int,int);
extern size_t ytext;
extern row_dword xtext;
//bar,tit
#define refreshpage(w) refreshrows(w,0)
extern char key_quit;
//tw
int xc_to_c(size_t,int);
extern char key_wrap;
//tw,tit
command_char movment(int,WINDOW*);
char orig_key(char,char);
#define orig_upkey(a) orig_key(a,0)
extern bool ocompiler_flag;
extern size_t aftercall;
//tit
void fixed_x(size_t,row_dword*,int,int);
extern char key_titles;

//bar
//main
void bar_init(void);
command_char save(void);
command_char saving_base(char*);
command_char question(const char*);
command_char change_key(bar_byte);
command_char change_tab_size(bar_byte);
command_char change_save_timeout(bar_byte);
command_char command(char*,show_key_struct);
#define centering2(w,prw,pxc,right) position(0,0);centering3(w,prw,pxc,right);
#define centering(w,prw,pxc) centering2(w,prw,pxc,false)
#define centering_simple(w) centering(w,nullptr,nullptr)
command_char go_to(bar_byte);
WINDOW*position_init(void);
void position(int,int);
void position_reset(void);
bool undo_add(size_t,row_dword,size_t,row_dword);
bool undo_add_del(size_t,row_dword,size_t,row_dword);
bool undo_add_ind(size_t,size_t);
bool undo_add_ind_del(size_t,size_t);
void undo_free(void);
void undo(WINDOW*);
void redo(WINDOW*);
void undo_save(void);
bool undo_type(size_t,row_dword,size_t,row_dword);
bool undo_bcsp(size_t,row_dword,size_t,row_dword);
bool undo_delk(size_t,row_dword,size_t,row_dword);
void undo_loop(WINDOW*);
bool new_visual(char*);
void err_set(WINDOW*);
int open_new(char*);
void texter_macro(const char*);
size_t aftercall_find();
void aftercall_draw(WINDOW*);
size_t init_aftercall();
//tw
void centering3(WINDOW*,size_t*,row_dword*,bool);
//tit
bool bar_char(char,WINDOW*,bool);
//main,tit
bool bar_clear(void);
//tw,tit
int centeringy(WINDOW*);
void position_core(size_t,row_dword);

//tw
//main
bool text_wrap(WINDOW*);
//tit
void extra_unlock(size_t,size_t,WINDOW*);
extern row*rowswrap;
extern row*store_rows;
extern size_t store_rows_tot;
extern size_t store_aftercall;

//tit
//main
bool titles(WINDOW*);

//split
//main
split_char split_conditions(char*,bool);
bool split_grab(char**,size_t*);
bool split_writeprefs(int);
bool split_readprefs(int);
void split_freeprefs();
bool split_write_init(char*);
const char* split_write(size_t*,int,unsigned int*,bool*);
void split_write_free();
extern bool splits_flag;
extern split_char split_reminder_c;
extern char*sdelimiter;
extern char*sdelimiter_new;
extern char*esdelimiter;
extern char*esdelimiter_new;
extern char*split_out;
extern char*split_out_new;
extern char*split_extension;
extern char*split_extension_new;
//bar
#ifndef HAVE_MEMMEM
void *memmem(const void*,size_t,const void*,size_t);
#endif
swrite_char swrite(int,void*,unsigned int);
