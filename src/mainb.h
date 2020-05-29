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

#define Char_Return 0xd
#define Char_Backspace 0x7f

bool no_char(char);
void refreshpage(WINDOW*w);
size_t c_to_xc(int,int);

char*bar_init(void);
int save(void);
int question(char*);
int command(char*);
void centering(WINDOW*,size_t*,size_t*);
WINDOW*position_init(void);
void position(int,int);
void position_reset(void);

#define tab_sz 6
extern char ln_term[3];
extern size_t ln_term_sz;
extern char*textfile;
extern row*rows;
extern size_t rows_tot;
extern size_t ytext;
extern size_t xtext;
