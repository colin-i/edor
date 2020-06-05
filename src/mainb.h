//#include<curses.h>
typedef void WINDOW;
int move(int,int);
int wmove(WINDOW*,int,int);
int getch(void);
int wgetch(WINDOW*);
extern WINDOW*stdscr;
int getmaxy(const WINDOW*);
int getmaxx(const WINDOW*);
int getcury(const WINDOW*);
int getcurx(const WINDOW*);
typedef unsigned int chtype;
int addch(const chtype);
int waddch(WINDOW*,const chtype);
int mvaddch(int,int,const chtype);
int addstr(const char*);
#define KEY_LEFT 0404
#define KEY_RIGHT 0405
#define KEY_DC 0512
#define KEY_RESIZE 0632
const char*keyname(int);
int wnoutrefresh(WINDOW*);
int attrset(int);
int COLOR_PAIR(int);
WINDOW*newwin(int,int,int,int);
//#include <string.h>
int strcmp(const char*,const char*);
//stdio.h
int sprintf(char*,const char*,...);

bool no_char(char);
void refreshpage(WINDOW*w);
size_t c_to_xc(int,int);
bool row_alloc(row*,size_t,size_t,size_t);
void row_set(row*,size_t,size_t,size_t,char*);
void deleting(size_t,size_t,size_t,size_t);
void mod_set(bool);
size_t sizemembuf(size_t,size_t,size_t,size_t);
void cpymembuf(size_t,size_t,size_t,size_t,char*);
bool paste(size_t,size_t,size_t*,char*,size_t,size_t,bool);

char*bar_init(void);
int save(void);
int question(char*);
int command(char*);
void centering(WINDOW*,int*,int*);
WINDOW*position_init(void);
void position(int,int);
void position_reset(void);
bool undo_add(size_t,size_t,size_t,size_t);
bool undo_add_del(size_t,size_t,size_t,size_t);
bool undo_add_ind(size_t,size_t);
bool undo_add_ind_del(size_t,size_t);
void undo_free(void);
void undo(WINDOW*w);

#define Char_Return 0xd
#define Char_Backspace 0x7f
#define row_pad 0xF
#define tab_sz 6

extern char ln_term[3];
extern size_t ln_term_sz;
extern char*textfile;
extern row*rows;
extern size_t rows_tot;
extern size_t ytext;
extern size_t xtext;
