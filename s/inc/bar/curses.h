
#include "inc/curses.h"

#ifdef __cplusplus
extern "C"{
#endif

int addchnstr(const chtype *chstr, int n);
int addnstr(const char*,int);
int getbegx(const WINDOW*);
int getbegy(const WINDOW*);
int mvaddnstr(int,int,const char*,int);
int mvaddstr(int,int,const char*);
int mvwaddstr(WINDOW*,int,int,const char*);
int mvwin(WINDOW*,int,int);
int wdeleteln(WINDOW*);
int wresize(WINDOW*,int,int);

#ifdef __cplusplus
}
#endif
