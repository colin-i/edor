#include"main0.h"
//strlen,2;open,2;close;write,3
#include"mainb.h"
//move,17;getch,2;getmaxy,2;getmaxx
//getcurx,6;stdscr,10;keyname;strcmp
//addch,10;mvaddch,7,addstr;wnoutrefresh,2
#include"mainbc.h"

//#include<curses.h>
int addnstr(const char*,int);//7
int mvaddstr(int,int,const char*);
int mvaddnstr(int,int,const char*,int);
//unistd.h
#define F_OK 0
int access(const char*,int);              
//string.h
//void*memset(void*,int,size_t);//3

static int com_left;
#define max_path 0xff
//10
static char input[max_path+1];

char*bar_init(){
	char*h="F1 for help";
	com_left=(int)(strlen(h)+1);
	return h;
}
static bool wrt(int f,row*rows,size_t sz){
	size_t n=sz-1;
	for(size_t i=0;i<n;i++){
		row*r=&rows[i];
		if((size_t)write(f,r->data,r->sz)!=r->sz)return false;
		if((size_t)write(f,ln_term,ln_term_sz)!=ln_term_sz)return false;
	}
	return (size_t)write(f,rows[n].data,rows[n].sz)==rows[n].sz;
}
static int bcdl(int y,int*p,int cursor){
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
static bool saving(char*path,row*rows,size_t sz,bool creat){
	int f;bool r;
	if(creat)f=open(path,O_CREAT|O_WRONLY|O_TRUNC,S_IRUSR|S_IWUSR);
	else f=open(path,O_WRONLY|O_TRUNC);
	if(f!=-1){
		r=wrt(f,rows,sz);
		close(f);
	}else r=false;
	return r;
}
//-1exist,0er,1ok
static int saves(char*path,row*rows,size_t sz){
	if(access(path,F_OK)==-1){
		return saving(path,rows,sz,true);
	}
	return -1;
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
int question(char*q){
	//if(com_left+sz>getmaxx(stdscr))return 1;
	int y=getmaxy(stdscr)-1;
	mvaddstr(y,com_left,q);
	addstr("? Y/c/n");
	int ch=getch();
	if(ch==KEY_RESIZE)return -2;
	move(y,com_left);
	int sz=(int)(strlen(q)+sizeof("? Y/c/n"));
	for(int i=0;i<sz;i++)
		addch(' ');
	//memset(mapsel,' ',sz);mapsel[sz]=0;mvaddstr(y,com_left,mapsel);
	if(ch=='c')return 0;
	else if(ch=='n')return -1;
	return 1;
}
static int del(int x,int cursor,int dif){
	if(x==cursor)return cursor;
	cursor--;
	for(int i=x;i<cursor;i++){
		input[i]=input[i+1];
	}
	int f=x+dif;int r;
	if(cursor<f)r=cursor;else r=f;
	while(x<r){addch(input[x]);x++;}
	if(cursor<f){
		addch(' ');
	}else if(f==cursor)addch(' ');
	return cursor;
}
//-2resize,-1no/quit,0er,1ok
int save(row*rows,size_t sz,char**path){
	if(path[0]){
		return saving(path[0],rows,sz,false);
	}
	int right=getmaxx(stdscr)-4;//25
	int rightexcl=right+1;
	int visib=rightexcl-com_left;
	if(visib<2)return 0;//phisical visib is 1
	int y=getmaxy(stdscr)-1;
	move(y,com_left);
	int cursor=0;
	int pos=0;int r;
	for(;;){
		int a=getch();
		if(a==Char_Return){
			input[cursor]=0;
			path[0]=input;
			r=saves(input,rows,sz);
			if(r==-1){
				int x=getcurx(stdscr);
				clear_com(y,visib,pos,cursor);
				r=question("Overwrite");
				if(r==1){
					r=saving(input,rows,sz,false);
				}else if(!r){
					if(pos)mvaddch(y,com_left-1,'<');
					else move(y,com_left);
					int len=cursor-pos;
					bool rt=len>visib;
					if(rt)len=visib;
					addnstr(input+pos,len);
					if(rt)addch('>');
					move(y,x);
					continue;
				}else if(r==-2)return -2;
				wnoutrefresh(stdscr);
				return r;
			}
			break;
		}
		else if(a==Char_Backspace){
			cursor=bcdl(y,&pos,cursor);
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
		else if(a==KEY_DC){
			int x=getcurx(stdscr);
			cursor=del(x-com_left+pos,cursor,rightexcl-x);
			move(y,x);
		}
		else if(a==KEY_RESIZE)return -2;
		else{
			const char*s=keyname(a);
			if(!strcmp(s,"^Q")){r=-1;break;}
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
	return r;
}
