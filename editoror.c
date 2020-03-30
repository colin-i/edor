//#include <string.h>
typedef unsigned int size_t;
size_t strlen(const char*);//5
char*strstr(const char*,const char*);
//#include <fcntl.h>
int open(const char*,int,...);
//sys/types.h
typedef unsigned short mode_t;
//asm-generic/fcntl.h
#define O_RDONLY 00000000
#define O_WRONLY 00000001
#define O_CREAT 00000100
#define O_TRUNC 00001000
//linux/stat.h
#define S_IRUSR 00400
#define S_IWUSR 00200
//#include <unistd.h>
int close(int);
typedef int ssize_t;
ssize_t write(int,const void*,size_t);//2
typedef long off_t;
off_t lseek(int,off_t,int);
ssize_t read(int,void*,size_t);
//#include <bits/seek_constants.h>
#define SEEK_SET 0
#define SEEK_END 2
//#include<curses.h>
typedef void WINDOW;
WINDOW*initscr(void);
int endwin(void);
int wgetch(WINDOW*);//2
int getch(void);
typedef unsigned mmask_t;
mmask_t mousemask(mmask_t,mmask_t*);
int noecho(void);
#define ALL_MOUSE_EVENTS 0xfffff
int wmove(WINDOW*,int,int);//5
int move(int,int);//2
int getcury(const WINDOW*);//3
int getcurx(const WINDOW*);//3
int getmaxy(const WINDOW*);//4
int getmaxx(const WINDOW*);//3
WINDOW*newwin(int,int,int,int);
int doupdate(void);
int wnoutrefresh(WINDOW*);//4
int mvprintw(int,int,const char*,...);//3
int mvwprintw(WINDOW*,int,int,const char*,...);//2
extern WINDOW*stdscr;
int werase(WINDOW*);
int clrtoeol(void);
//#include<poll.h>
typedef unsigned int nfds_t;
struct pollfd{
	int fd;
	short events;
	short revents;
};
int poll(struct pollfd[],nfds_t,int);
#define POLLIN 0x0001
struct pollfd stdinfd={0,POLLIN,0};
//#include <stdlib.h>
void*malloc(size_t);//2
void free(void*);//2
void*realloc(void*,size_t);
//#include<stdio.h>
int printf(const char*,...);//5
int getchar(void);

#define NULL 0
typedef enum{true=1,false=0}bool;
typedef struct{
	char*str;
	char**t;
}inst;

char ln_term[3]="\n";
int ln_term_sze;
char*text_w=NULL;
int help_rows;

char terms[][3]={"r0"};
char*_r0_0[]={terms[0],"#0",0};char*_ex[]={"exit",0};
inst instrs[]={
	{"mov",_r0_0},{"b",_ex},0
};
bool out_chr(int f,char c){
	ssize_t s=write(f,&c,1);
	if(s!=1)return true;
	return false;
}
bool out_wrt(int f,char*c){
	int n=strlen(c);
	ssize_t s=write(f,c,n);
	if(s!=n)return true;
	return false;
}
void out_wr(int f){
	if(out_wrt(f,".global main\n"))return;
	if(out_wrt(f,".extern exit\n"))return;
	if(out_wrt(f,"main:\n"))return;
	int i=0;
	do{
		inst*a=&instrs[i];
		if(a->str==0)return;
		if(out_wrt(f,a->str))return;
		if(out_chr(f,' '))return;
		if(out_wrt(f,a->t[0]))return;
		int j=1;
		do{
			char*b=a->t[j];
			if(b==0)break;
			if(out_chr(f,','))return;
			if(out_wrt(f,b))return;
			j++;
		}while(true);
		if(out_chr(f,'\n'))return;
		i++;
	}while(true);
}
int ach(WINDOW*w){
	if(poll(&stdinfd,1,0)<1)return 0;
	return wgetch(w);
}
int nr_end_test(WINDOW*w,char t){
	int nr=0;
	int x=ach(w);
	if(x==t)return -1;
	while(x>47&&x<58){
		nr*=10;
		nr+=x-48;
		x=ach(w);
	}
	if(x!=t)return -1;
	return nr;
}
int mouse_test(WINDOW*w,int*x,int*y){
	int a=nr_end_test(w,';');
	if(a==-1)return -1;
	x[0]=nr_end_test(w,';');
	if(x[0]==-1)return -1;
	//if(x>maxx)return -1;
	y[0]=nr_end_test(w,'M');
	if(y[0]==-1)return -1;
	if(y[0]>getmaxy(w))return -1;
	return a;
}
void tab_grow(WINDOW*w,int r,char*a){
	int max=getmaxx(w);int c=0;
	int sz=strlen(a);int j=0;int i=0;
	for(;i<sz&&c<max;i++){
		char z=a[i];
		if(z=='\t'){
			a[i]=0;mvwprintw(w,r,c,a+j);a[i]='\t';
			c+=i-j+6;sz-=5;j=i+1;
		}else if(z<32||z>=127){
			a[i]='?';char aux=a[i+1];a[i+1]=0;
			mvwprintw(w,r,c,a+j);a[i+1]=aux;a[i]=z;
			c+=i-j+1;j=i+1;
		}
	}
	if(c<max){
		char e=a[i];
		a[i]=0;mvwprintw(w,r,c,a+j);a[i]=e;
	}
}
void printpage(WINDOW*w,char*str){
	int i=0;int maxy=getmaxy(w);
	int maxx=getmaxx(w);
	do{
		char store;int jump;char*b=NULL;
		char*a=strstr(str,ln_term);
		if(a){
			int sz=a-str;jump=sz+ln_term_sze;
			if(sz>maxx)b=str+maxx;
			else b=a;
		}else if(strlen(str)>maxx)b=str+maxx;
		if(b){store=b[0];b[0]=0;}
		tab_grow(w,i,str);
		if(b)b[0]=store;
		if(a)str+=jump;
		else break;
		i++;
	}while(i<maxy);
}
void helpin(WINDOW*w){
	int c;
	do{
		c=getch();
	}while(c!='q');
	for(int i=0;i<help_rows;i++){
		move(i,0);
		clrtoeol();
	}
	wnoutrefresh(stdscr);
	printpage(w,text_w);
}
void helprows(char*s){
	mvprintw(help_rows,0,s);
	help_rows++;
}
void loopin(WINDOW*w){
	int c;
	do{
		c=wgetch(w);
		if(c=='\e'){
			c=ach(w);if(c=='['){
				c=ach(w);if(c=='<'){
					int x;int y;
					int m=mouse_test(w,&x,&y);
					if(m==0){wmove(w,y-1,x-1);}
					else if(m==64){
						y=getcury(w);
						if(y<getmaxy(w))wmove(w,y+1,getcurx(w));
					}
					else if(m==65){
						y=getcury(w);
						if(y>0)wmove(w,y-1,getcurx(w));
					}
				}
			}
		}
		else if(c=='h'){
			//if((getcurx(stdscr)|getcury(stdscr))==0){
			int cy=getcury(w);int cx=getcurx(w);
			werase(w);
			help_rows=0;
			helprows("q is for quitting");
			helprows("mouse/touch press or v.scroll");
			wnoutrefresh(w);
			//}
			wnoutrefresh(stdscr);
			doupdate();
			helpin(w);
			wmove(w,cy,cx);
		}
	}while(c!='q');
}
int normalize(){
	int ok=0;
	int sz=strlen(text_w);
	char*norm=(char*)malloc(2*sz-1);
	if(norm){
		int j=0;
		for(int i=0;i<sz;i++){
			char a=text_w[i];
			if(a=='\n'){
				if(ln_term_sze==2){if(i==0||text_w[i-1]!='\r'){norm[j]='\r';j++;ok=-1;}}
				else if(ln_term[0]=='\r'){a='\r';ok=-1;}
			}else if(a=='\r'){
				if(ln_term_sze==2){if(i+1==sz||text_w[i+1]!='\n'){norm[j]=a;j++;a='\n';ok=-1;}}
				else if(ln_term[0]=='\n'){a='\n';ok=-1;}
			}
			norm[j]=a;j++;
		}
		norm[j]=0;
		free(text_w);text_w=norm;
		if(!ok)ok=1;
	}
	return ok;
}
int startpage(char*f){
	int ok=1;
	int fd=open(f,O_RDONLY);
	if(fd!=-1){
		int size=lseek(fd,0,SEEK_END);
		char*tx=realloc(text_w,size+1);
		if(tx){
			text_w=tx;
			lseek(fd,0,SEEK_SET);
			read(fd,text_w,size);
			text_w[size]=0;
			//
			for(int i=size-1;i>-1;i--){
				if(text_w[i]=='\n'){
					if(i&&text_w[i-1]=='\r'){
						ln_term[0]='\r';
						ln_term[1]='\n';
						ln_term[2]=0;
						break;
					}
					else break;
				}else if(text_w[i]=='\r'){
					ln_term[0]='\r';
					break;
				}
			}
			ln_term_sze=strlen(ln_term);
			//
			ok=normalize();
		}
		close(fd);
	}
	return ok;
}
void out_f(char*str){
	int n=strlen(str);
	char*ext=".s";
        int e=strlen(ext);
	int p=n+e+1;
	char*s=(char*)malloc(p);
	if(s){
		p--;
		s[p]=0;
		while(e){
			e--;p--;
			s[p]=ext[e];
		}
		while(p){
			p--;
			s[p]=str[p];
		}
		int f=open(s,O_WRONLY|O_CREAT|O_TRUNC,S_IRUSR|S_IWUSR);
		if(f!=-1){
			out_wr(f);
			close(f);
		}
		free(s);
	}
}
int main(int argc,char**argv){
	text_w=(char*)malloc(1);
	if(text_w){
		text_w[0]=0;
		int ok=1;
		if(argc==2){
			ok=startpage(argv[1]);
			if(ok){
				if(ok<1){
					printf("Normalize line endings to ");
					if(ln_term_sze==2)printf("\\r\\n");
					else if(ln_term[0]=='\n')printf("\\n");
					else printf("\\r");
					printf("? n=no, default=yes\n");
					int c=getchar();
					if(c=='n')ok=0;
				}
				if(ok)out_f(argv[1]);
			}
		}
		if(ok){
			WINDOW*w1=initscr();
			noecho();
			mousemask(ALL_MOUSE_EVENTS,NULL);
			int y=getmaxy(w1);
			WINDOW*w=newwin(y-1,getmaxx(w1),0,0);
			printpage(w,text_w);
			wmove(w,0,0);
			mvprintw(y-1,0,"h for help");
			wnoutrefresh(w1);
			move(0,0);
			loopin(w);
			endwin();
		}
	}
	free(text_w);
}
