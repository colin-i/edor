#include"main0.h"
//strlen;open;close;write,3
#include"main2.h"
//move,11;getch;getmaxy;getmaxx;getcurx,4
//stdscr,6;keyname;strcmp;mvaddch,6

//#include<curses.h>
int addch(const chtype);//6
int addnstr(const char*,int);//6
int mvaddnstr(int,int,const char*,int);

static int com_left;

//#define max_path 256
#define max_path 10

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
static int bcdl(int y,char*input,int*p,int cursor){
	int x=getcurx(stdscr);
	bool left=x==com_left;
	int pos=p[0];
	if(!pos&&left)return cursor;
	int of=x-com_left;
	for(int i=pos+of;i<cursor;i++){
		input[i-1]=input[i];
	}
	if(!pos){
		x--;
		if(of==cursor){
			mvaddch(y,x,' ');
		}else{
			mvaddnstr(y,x,input+of-1,cursor-of);
			addch(' ');
		}
		move(y,x);
		return cursor-1;
	}
	if(!left){
		pos--;
		if(!pos)mvaddch(y,com_left-1,' ');
		else move(y,com_left);
		addnstr(input+pos,x-com_left);
		p[0]=pos;
		return cursor-1;
	}
	pos--;
	if(!pos)mvaddch(y,com_left-1,' ');
	p[0]=pos;
	return cursor-1;
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
//	int right=getmaxx(stdscr)-4;
	int right=getmaxx(stdscr)-25;
	int visib=right-com_left+1;
	int cursor=0;char input[max_path];
	int pos=0;
	for(;;){
		int a=getch();
		if(a==Char_Return)return false;
		else if(a==Char_Backspace){
			cursor=bcdl(y,input,&pos,cursor);
		}
		else if(a==KEY_LEFT){
			int x=getcurx(stdscr);
			if(x>com_left)
				move(y,x-1);
			else if(pos>0){
				pos--;
				addnstr(input+pos,visib);
				if(pos+visib==cursor-1)addch('>');
				if(!pos)mvaddch(y,com_left-1,' ');
				else move(y,com_left);
			}
		}
		else if(a==KEY_RIGHT){
			int x=getcurx(stdscr);
			if(x<right){if(x<com_left+cursor)move(y,x+1);}
			else if(pos+visib<=cursor){
				if(!pos)mvaddch(y,com_left-1,'<');
				else move(y,com_left);
				if(pos+visib==cursor){
					pos++;
					addnstr(input+pos,visib-1);
					addch(' ');
				}
				else{
					pos++;
					addnstr(input+pos,visib);
					if(pos+visib==cursor)addch(' ');
				}
				move(y,x);
			}
		}
		else if(a==KEY_RESIZE)return true;
		else{
			const char*s=keyname(a);
			if(!strcmp(s,"^Q"))return false;
			if(cursor==max_path)continue;
			char ch=(char)a;
			if(!no_char(ch)){
				int x=getcurx(stdscr);
				int off=pos+(x-com_left);
				for(int i=cursor;i>off;i--){
					input[i]=input[i-1];
				}
				input[off]=ch;
				int dif=right-x;
				if(!dif){
					if(!pos)mvaddch(y,com_left-1,'<');
					else move(y,com_left);
					pos++;
					addnstr(input+pos,visib-1);
				}else addch(ch);
				int d=cursor-off;
				if(d){
					int n=right-x;
					if(dif)x++;else n++;
					if(d<n)n=d;
					int i=off+1;
					addnstr(input+i,n);
					if(i+n==cursor)addch('>');
					move(y,x);
				}
				cursor++;
			}
		}
	}
}
