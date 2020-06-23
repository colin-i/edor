
typedef void WINDOW;
extern WINDOW*stdscr;
typedef unsigned int chtype;
#define KEY_LEFT 0404
#define KEY_RIGHT 0405
#define KEY_DC 0512
#define KEY_RESIZE 0632

#ifdef __cplusplus
extern "C"{
#endif

int move(int,int);
int wmove(WINDOW*,int,int);
int getch(void);
int wgetch(WINDOW*);
int getmaxy(const WINDOW*);
int getmaxx(const WINDOW*);
int getcury(const WINDOW*);
int getcurx(const WINDOW*);
int addch(const chtype);
int waddch(WINDOW*,const chtype);
int mvaddch(int,int,const chtype);
int addstr(const char*);
const char*keyname(int);
int wnoutrefresh(WINDOW*);
int attrset(int);
int wattrset(WINDOW*,int);
int COLOR_PAIR(int);
WINDOW*newwin(int,int,int,int);

int mvwaddch(WINDOW*,int,int,const chtype);
int addnstr(const char*,int);
int mvaddstr(int,int,const char*);
int mvaddnstr(int,int,const char*,int);
int mvwaddstr(WINDOW*,int,int,const char*);
int wresize(WINDOW*,int,int);
int mvwin(WINDOW*,int,int);
int getbegx(const WINDOW*);
int getbegy(const WINDOW*);
int wdeleteln(WINDOW*);

#ifdef __cplusplus
}
#endif
