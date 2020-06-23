
typedef void WINDOW;
extern WINDOW*stdscr;
typedef unsigned int chtype;
#define KEY_LEFT 0404
#define KEY_RIGHT 0405
#define KEY_DC 0512
#define KEY_RESIZE 0632

#define ALL_MOUSE_EVENTS 0xFffFFff
#define COLOR_BLACK 0
#define COLOR_CYAN 6
#define COLOR_WHITE 7
#define ERR -1
typedef unsigned long mmask_t;
typedef struct
{
	short id;// __attribute__((aligned(4)));
	int x,y,z;
	mmask_t bstate;
}
MEVENT;
//#define OK 0
#define BUTTON1_CLICKED 0x4
#define BUTTON4_PRESSED 0x10000
#define BUTTON5_PRESSED 0x200000
#define KEY_UP 0403
#define KEY_DOWN 0402
#define KEY_HOME 0406
#define KEY_SF 0520
#define KEY_SR 0521
#define KEY_NPAGE 0522
#define KEY_PPAGE 0523
#define KEY_END 0550
#define KEY_SLEFT 0611
#define KEY_SRIGHT 0622
#define KEY_MOUSE 0631

#ifdef __cplusplus
extern "C" {
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

WINDOW*initscr(void);
int endwin(void);
int ungetch(int);
chtype winch(WINDOW*);
int winnstr(WINDOW*,char*,int);
mmask_t mousemask(mmask_t,mmask_t*);
int noecho(void);
int raw(void);
int nonl(void);
int delwin(WINDOW*);
int waddstr(WINDOW*,const char*);
int waddnstr(WINDOW*,const char*,int);
int clrtoeol(void);
int wclrtoeol(WINDOW*);
int start_color(void);
int init_pair(short,short,short);
int keypad(WINDOW*,bool);
int getmouse(MEVENT*);
int nodelay(WINDOW*,bool);

#ifdef __cplusplus
}
#endif
