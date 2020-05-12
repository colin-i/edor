#include"main0.h"
//strlen;open;close;write,3
#include"main2.h"
//move,9;getch;getmaxy;getmaxx;getcurx,3
//stdscr,5;keyname;strcmp

//#include<curses.h>
int addch(const chtype);//3
int addnstr(const char*,int);//4

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
		else if(a==Char_Backspace){}
		else if(a==KEY_LEFT){
			int x=getcurx(stdscr);
			if(x>com_left)
				move(y,x-1);
			else if(pos>0){
				pos--;
				addnstr(input+pos,visib);
				move(y,com_left);
			}
		}
		else if(a==KEY_RIGHT){
			int x=getcurx(stdscr);
			if(x<right){if(x<com_left+cursor)move(y,x+1);}
			else if(pos+visib<=cursor){
				move(y,com_left);
				if(pos+visib==cursor){
					pos++;
					addnstr(input+pos,visib-1);
					addch(' ');
				}
				else{
					pos++;
					addnstr(input+pos,visib);
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
					move(y,com_left);
					pos++;
					addnstr(input+pos,visib-1);
				}else addch(ch);
				if(off<cursor){
					int i=off+1;
					int limit=off+dif;
					while(i<=cursor){
						addch(input[i]);
						if(i>=limit)break;//when x==right,i==limit is not enough
						//if(x==right)break;else x++;
						i++;
					}
					move(y,dif?x+1:x);
				}
				cursor++;
			}
		}
	}
}
