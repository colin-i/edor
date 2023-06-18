
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
int move(int,int);
int wmove(WINDOW*,int,int);
int getch(void);
int getmaxy(const WINDOW*);
int addch(const chtype);
int waddch(WINDOW*,const chtype);
int mvaddch(int,int,const chtype);
int mvwaddch(WINDOW*,int,int,const chtype);
int addstr(const char*);
const char*keyname(int);
int wnoutrefresh(WINDOW*);
int attrset(int);
int wattrset(WINDOW*,int);
int COLOR_PAIR(int);
WINDOW*newwin(int,int,int,int);
//main/bar/tw
int getcurx(const WINDOW*);
int getcury(const WINDOW*);
int getmaxx(const WINDOW*);
int wgetch(WINDOW*);

#ifdef __cplusplus
}
#endif
