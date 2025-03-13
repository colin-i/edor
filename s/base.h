
//main
//bar
size_t c_to_xc(int,int);
void cpymembuf(size_t,size_t,size_t,size_t,char*);
void deleting(size_t,size_t,size_t,size_t);
bool deleting_init(size_t,size_t,size_t,size_t);
void editing_rebase();
void mod_set_off();
void mod_set_off_wrap();
void mod_set_on();
bool no_char(char);
bool paste(size_t,size_t,size_t*,char*,size_t,size_t,bool);
void pref_modify(char**,char**,bool,char*,size_t);
void printinverted(const char*s);
void refreshrowsbot(WINDOW*,int,int);
#define refreshrows(w,i) refreshrowsbot(w,i,getmaxy(w))
void restore_rebase();
bool row_alloc(row*,size_t,size_t,size_t);
void row_set(row*,size_t,size_t,size_t,const char*);
size_t sizemembuf(size_t,size_t,size_t,size_t);
void vis(char,WINDOW*);
//tw
int xc_to_c(size_t,int);
//tit
void fixed_x(size_t,size_t*,int,int);
//bar,tit
#define refreshpage(w) refreshrows(w,0)
//bar,tw,tit
void visual(char);
void fixed_yx(size_t*,size_t*,int,int);
//tw,tit
int movment(int,WINDOW*);

//bar
//main
void bar_init(void);
int save(void);
int saving_base(char*);
int question(const char*);
int command(char*);
#define centering2(w,prw,pxc,right) position(0,0);centering3(w,prw,pxc,right);
#define centering(w,prw,pxc) centering2(w,prw,pxc,false)
#define centering_simple(w) centering(w,nullptr,nullptr)
WINDOW*position_init(void);
void position(int,int);
void position_reset(void);
bool undo_add(size_t,size_t,size_t,size_t);
bool undo_add_del(size_t,size_t,size_t,size_t);
bool undo_add_ind(size_t,size_t);
bool undo_add_ind_del(size_t,size_t);
void undo_free(void);
void undo(WINDOW*);
void redo(WINDOW*);
void undo_save(void);
bool undo_type(size_t,size_t,size_t,size_t);
bool undo_bcsp(size_t,size_t,size_t,size_t);
bool undo_delk(size_t,size_t,size_t,size_t);
void undo_loop(WINDOW*);
bool new_visual(char*);
bool is_dir(int);
void err_set(WINDOW*);
int open_new(char*);
void texter_macro(const char*);
size_t aftercall_find();
void aftercall_draw(WINDOW*);
size_t init_aftercall();
//tw
void position_core(size_t,size_t);
void centering3(WINDOW*,size_t*,size_t*,bool);
//tit
void bar_char(char,WINDOW*);
//main,tit
bool bar_clear(void);
//tw,tit
int centeringy(WINDOW*);

//tw
//main
bool text_wrap(WINDOW*);
//tit
void extra_unlock(size_t,size_t,WINDOW*);

//tit
//main
bool titles(WINDOW*);

//split
//main
bool split_grab(char**,size_t*);
void split_writeprefs(int);
void split_readprefs(int);
void split_freeprefs();
bool split_write_init(char*);
char* split_write(size_t*,int,unsigned int*,bool*);
void split_write_free();
//bar
bool swrite(int,void*,unsigned int);

//main
//bar
extern char*textfile;
extern bool mod_flag;
extern bool insensitive;
extern char*ocode_extension;
//bar,tw,tit
extern size_t ytext;
extern size_t xtext;
//tw,tit
extern bool ocompiler_flag;
extern size_t aftercall;

//tw
//tit
extern row*rowswrap;
extern row*store_rows;
extern size_t store_rows_tot;
extern size_t store_aftercall;

//split
//main
extern bool splits_flag;
extern char*sdelimiter;
extern char*sdelimiter_new;
extern char*esdelimiter;
extern char*esdelimiter_new;
extern char*split_out;
extern char*split_out_new;

#include "def.h"
