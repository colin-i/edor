
#include "inc/curses.h"

#ifdef __cplusplus
extern "C"{
#endif

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
