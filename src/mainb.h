//#include<curses.h>
typedef void WINDOW;
int move(int,int);
int wmove(WINDOW*,int,int);
int getch(void);
extern WINDOW*stdscr;
int getmaxy(const WINDOW*);
int getmaxx(const WINDOW*);
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
//#include <string.h>
int strcmp(const char*,const char*);

#define Char_Return 0xd
#define Char_Backspace 0x7f

bool no_char(char);
void refreshpage(WINDOW*w);

char*bar_init(void);
int save(void);
int question(char*);
int command(size_t*);
void centering(WINDOW*w);

#define tab_sz 6
extern char ln_term[3];
extern size_t ln_term_sz;
extern char*textfile;
extern row*rows;
extern size_t rows_tot;
extern size_t ytext;
extern size_t xtext;
