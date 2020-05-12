//#include<curses.h>
int move(int,int);
int getch(void);
typedef unsigned int chtype;
typedef void WINDOW;
extern WINDOW*stdscr;
int getmaxy(const WINDOW*);
int getmaxx(const WINDOW*);
int getcurx(const WINDOW*);
#define KEY_LEFT 0404
#define KEY_RIGHT 0405
#define KEY_RESIZE 0632
const char*keyname(int);
//#include <string.h>
int strcmp(const char*,const char*);

#define Char_Return 0xd
#define Char_Backspace 0x7f

bool no_char(char);

bool save(row*,size_t,char*);
char*bar_init(void);

extern char ln_term[3];
extern size_t ln_term_sz;
