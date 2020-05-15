#include"main0.h"
//strlen,2;open,2;close;write,3
#include"mainb.h"
//move,16;getch,2;getmaxy;getmaxx,2
//getcurx,5;stdscr,9;keyname;strcmp
//mvaddch,7,addstr;mvaddstr;wnoutrefresh,2
#include"mainbc.h"

//#include<curses.h>
int addch(const chtype);//7
int addnstr(const char*,int);//7
int mvaddnstr(int,int,const char*,int);
//unistd.h
#define F_OK 0
int access(const char*,int);              
//string.h
//void*memset(void*,int,size_t);//3

static int com_left;
#define max_path 0xff
//10

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
static void saving(char*path,row*rows,size_t sz,bool creat){
	int f;
	if(creat)f=open(path,O_CREAT|O_WRONLY|O_TRUNC,S_IRUSR|S_IWUSR);
	else f=open(path,O_WRONLY|O_TRUNC);
	if(f!=-1){
		wrt(f,rows,sz);
		close(f);
	}
}
static bool saves(char*path,row*rows,size_t sz){
	if(access(path,F_OK)==-1){
		saving(path,rows,sz,true);
		return false;
	}
	return true;
}
static void clear_com(int y,int sz,int pos,int cursor){
	int len;if(pos){
		move(y,com_left-1);
		len=1+cursor-pos;sz++;
	}
	else{
		move(y,com_left);
		len=cursor;
	}
	if(len>sz){
		sz++;
		for(int i=0;i<sz;i++)
			addch(' ');
		//memset(mapsel,' ',(size_t)sz+1);mapsel[sz+1]=0;addstr(mapsel);
	}
	else{
		for(int i=0;i<len;i++)
			addch(' ');
		//memset(mapsel,' ',(size_t)len);mapsel[len]=0;addstr(mapsel);
	}
}
static int question(char*q,int y){
	int sz=(int)(strlen(q)+sizeof("? y/n/C"));
	if(com_left+sz>getmaxx(stdscr))return 1;
	mvaddstr(y,com_left,q);
	addstr("? Y/n/c");
	int ch=getch();
	move(y,com_left);
	for(int i=0;i<sz;i++)
		addch(' ');
	//memset(mapsel,' ',sz);mapsel[sz]=0;mvaddstr(y,com_left,mapsel);
	if(ch=='n')return 0;
	else if(ch=='c')return -1;
	return 1;
}
bool save(row*rows,size_t sz,char*path){
	if(path){
		saving(path,rows,sz,false);
		return false;
	}
	int right=getmaxx(stdscr)-4;//25
	int visib=right+1-com_left;
	if(visib<2)return false;//phisical visib is 1
	int y=getmaxy(stdscr)-1;
	move(y,com_left);
	int cursor=0;char input[max_path+1];
	int pos=0;
	for(;;){
		int a=getch();
		if(a==Char_Return){
			input[cursor]=0;
			if(saves(input,rows,sz)){
				int x=getcurx(stdscr);
				clear_com(y,visib,pos,cursor);
				int q=question("Overwrite",y);
				if(q==1){
					saving(input,rows,sz,false);
				}else if(q==-1){
					if(pos)mvaddch(y,com_left-1,'<');
					else move(y,com_left);
					int len=cursor-pos;
					bool rt=len>visib;
					if(rt)len=visib;
					addnstr(input+pos,len);
					if(rt)addch('>');
					move(y,x);
					continue;
				}
				wnoutrefresh(stdscr);
				return false;
			}
			break;
		}
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
			if(!strcmp(s,"^Q"))break;
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
	clear_com(y,visib,pos,cursor);
	wnoutrefresh(stdscr);
	return false;
}
