//#include<curses.h>
int move(int,int);
int getch(void);
typedef void WINDOW;
extern WINDOW*stdscr;
int getmaxy(const WINDOW*);
int getmaxx(const WINDOW*);
int getcurx(const WINDOW*);
typedef unsigned int chtype;
int mvaddch(int,int,const chtype);
int addstr(const char*);
int mvaddstr(int,int,const char*);
#define KEY_LEFT 0404
#define KEY_RIGHT 0405
#define KEY_RESIZE 0632
const char*keyname(int);
int wnoutrefresh(WINDOW*);
//#include <string.h>
int strcmp(const char*,const char*);

#define Char_Return 0xd
#define Char_Backspace 0x7f

bool no_char(char);

char*bar_init(void);
int save(row*,size_t,char**);
int question(char*);

extern char ln_term[3];
extern size_t ln_term_sz;