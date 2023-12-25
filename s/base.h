
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
void printinverted(const char*s);
void refreshrowsbot(WINDOW*,int,int);
#define refreshrows(w,i) refreshrowsbot(w,i,getmaxy(w))
#define refreshpage(w) refreshrows(w,0)
void restore_rebase();
bool row_alloc(row*,size_t,size_t,size_t);
void row_set(row*,size_t,size_t,size_t,const char*);
size_t sizemembuf(size_t,size_t,size_t,size_t);
void vis(char,WINDOW*);
//tw
int movment(int,WINDOW*);
int xc_to_c(size_t,int);
//bar,tw
void visual(char);
void fixed_yx(size_t*,size_t*,int,int);

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
bool bar_clear(void);
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
size_t centeringy(WINDOW*);

//tw
bool text_wrap(WINDOW*);

//main,bar
extern char ln_term[3];
extern size_t ln_term_sz;
extern char*textfile;
extern bool mod_flag;
extern bool issensitive;
extern char*ocode_extension;
//main,bar,tw
extern row*rows;
extern size_t rows_tot;
extern size_t ytext;
extern size_t xtext;
//main,tw
extern bool ocompiler_flag;
extern size_t aftercall;
