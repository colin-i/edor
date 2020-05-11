#include"main0.h"
//strlen;open;close;write,3
#include"main2.h"
//move,3;getch;getmaxy;getcurx,2;stdscr,3
//keyname;strcmp

//#include<curses.h>
int addch(const chtype);

static int com_left;

char*bar_init(){
	char*h="F1 for help";
	com_left=(int)(strlen(h)+1);
	return h;
}
static void wrt(int f,row*rows,size_t sz){
	size_t n=sz-1;
	for(size_t i=0;i<n;i++){
		row*r=&rows[i];
		if((size_t)write(f,r->data,r->sz)!=r->sz)return;
		if((size_t)write(f,ln_term,ln_term_sz)!=ln_term_sz)return;
	}
	write(f,rows[n].data,rows[n].sz);
}
bool save(row*rows,size_t sz,char*path){
	if(path){
		int f=open(path,O_WRONLY|O_TRUNC);
		if(f!=-1){
			wrt(f,rows,sz);
			close(f);
		}
		return false;
	}
	int y=getmaxy(stdscr)-1;
	move(y,com_left);
	for(;;){
		int a=getch();
		if(a==Char_Return)return false;
		else if(a==Char_Backspace){}
		else if(a==KEY_LEFT){
			int x=getcurx(stdscr);
			move(y,x-1);
		}
		else if(a==KEY_RIGHT){
			int x=getcurx(stdscr);
			move(y,x+1);
		}
		else if(a==KEY_RESIZE)return true;
		else{
			const char*s=keyname(a);
			if(!strcmp(s,"^Q"))return false;
			char ch=(char)a;
			if(!no_char(ch))addch(ch);
		}
	}
}
