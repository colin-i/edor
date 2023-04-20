
//main
//bar
bool no_char(char);
void refreshrowsbot(WINDOW*,int,int);
#define refreshrows(w,i) refreshrowsbot(w,i,getmaxy(w))
#define refreshpage(w) refreshrows(w,0)
bool row_alloc(row*,size_t,size_t,size_t);
void row_set(row*,size_t,size_t,size_t,const char*);
bool deleting_init(size_t,size_t,size_t,size_t);
void deleting(size_t,size_t,size_t,size_t);
void mod_set_on();
void mod_set_off();
void mod_set_off_wrap();
size_t sizemembuf(size_t,size_t,size_t,size_t);
void cpymembuf(size_t,size_t,size_t,size_t,char*);
bool paste(size_t,size_t,size_t*,char*,size_t,size_t,bool);
void editing_rebase();
void restore_rebase();
void vis(char,WINDOW*);
size_t c_to_xc(int,int);
//tw
int movment(int,WINDOW*);
int xc_to_c(size_t,int);
//bar,tw
void visual(char);
void fixed_yx(size_t*,size_t*,int,int);

//bar
//main
const char*bar_init(void);
int save(void);
int saving_base(char*);
int question(const char*);
int command(char*);
void centering2(WINDOW*,size_t*,size_t*,bool);
#define centering(w,rw,cl) centering2(w,rw,cl,false)
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
//tw
void position_core(size_t,size_t);
void centering3(WINDOW*,size_t*,size_t*,bool);

//tw
bool word_wrap(WINDOW*);

//main,bar
extern char ln_term[3];
extern size_t ln_term_sz;
extern char*textfile;
extern bool mod_flag;
extern bool insensitive;
//main,bar,tw
extern row*rows;
extern size_t rows_tot;
extern size_t ytext;
extern size_t xtext;
