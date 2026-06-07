
#define ERR -1

typedef void WINDOW;
extern WINDOW*stdscr;
typedef unsigned int chtype;
#define KEY_LEFT 0404
#define KEY_RIGHT 0405
#define KEY_HOME 0406
#define KEY_BACKSPACE 0407
#define KEY_DC 0512
#define KEY_END 0550
#define KEY_RESIZE 0632

#ifdef __cplusplus
extern "C" {
#endif

//main/bar
int addch(const chtype);
int addstr(const char*);
int getch(void);
int getmaxy(const WINDOW*);
int init_pair(short,short,short);
int move(int,int);
int mvaddch(int,int,const chtype);
int mvwaddch(WINDOW*,int,int,const chtype);
int waddch(WINDOW*,const chtype);
int wattrset(WINDOW*,int);
int wnoutrefresh(WINDOW*);
WINDOW*newwin(int,int,int,int);
//main/bar/tw
int getmaxx(const WINDOW*);
//main/bar/tit
int attrset(int);
int COLOR_PAIR(int);
const char*keyname(int);
//main/bar/tw/tit
int getcurx(const WINDOW*);
int getcury(const WINDOW*);
int wgetch(WINDOW*);
int wmove(WINDOW*,int,int);

#ifdef __cplusplus
}
#endif
