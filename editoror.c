//#include <string.h>
typedef unsigned int size_t;
extern size_t strlen(const char*);//3
extern char*strchr(const char*,int);
extern char*strrchr(const char*,int);
extern char*strstr(const char*,const char*);
//#include <fcntl.h>
extern int open(const char*,int,...);
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
extern int close(int);
typedef int ssize_t;
extern ssize_t write(int,const void*,size_t);//2
typedef long off_t;
extern off_t lseek(int,off_t,int);
extern ssize_t read(int,void*,size_t);
//#include <bits/seek_constants.h>
#define SEEK_SET 0
#define SEEK_END 2
//#include<curses.h>
typedef void WINDOW;
extern WINDOW*initscr(void);
extern int endwin(void);
extern int wgetch(WINDOW*);//2
typedef unsigned mmask_t;
extern mmask_t mousemask(mmask_t,mmask_t*);
extern int noecho(void);
#define ALL_MOUSE_EVENTS 0xfffff
extern int wmove(WINDOW*,int,int);//4
extern int getcury(const WINDOW*);//3
extern int getcurx(const WINDOW*);//3
extern int getmaxy(const WINDOW*);//4
extern int getmaxx(const WINDOW*);//2
extern WINDOW*newwin(int,int,int,int);
extern int printw(const char*,...);
extern int doupdate(void);
extern int wnoutrefresh(WINDOW*);//2
extern int mvprintw(int,int,const char*,...);
extern WINDOW*stdscr;//2
extern int move(int,int);
extern int mvwprintw(WINDOW*,int,int,const char*,...);
extern int werase(WINDOW*);
extern int wrefresh(WINDOW*);
//#include<poll.h>
typedef unsigned int nfds_t;
struct pollfd{
	int fd;
	short events;
	short revents;
};
extern int poll(struct pollfd[],nfds_t,int);
#define POLLIN 0x0001
struct pollfd stdinfd={0,POLLIN,0};
//#include <stdlib.h>
void*malloc(size_t);
void free(void*);

#define NULL 0
typedef enum{true=1,false=0}bool;
typedef struct{
	char*str;
	char**t;
}inst;

char ln_term[3]="\n";
int ln_term_sze;

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
			if((getcurx(stdscr)|getcury(stdscr))==0){
				werase(w);wrefresh(w);
				printw("q is for quitting");
			}
			wnoutrefresh(stdscr);doupdate();
		}
	}while(c!='q');
}
void printpage(WINDOW*w,char*str){
	int i=0;int maxy=getmaxy(w);
	int maxx=getmaxx(w);
	do{
		char store;int jump;
		char*a=strstr(str,ln_term);
		if(a){
			int sz=a-str;jump=sz+ln_term_sze;
			if(sz>maxx)a=str+maxx;
		}else if(strlen(str)>maxx)a=str+maxx;
		if(a){store=a[0];a[0]=0;}
		mvwprintw(w,i,0,str);
		if(a){a[0]=store;str+=jump;}
		else break;
		i++;
	}while(i<maxy);
}
void startpage(WINDOW*w,char*f){
	int fd=open(f,O_RDONLY);
	if(fd!=-1){
		int size=lseek(fd,0,SEEK_END);
		char*str=malloc(size+1);
		if(str){
			lseek(fd,0,SEEK_SET);
			read(fd,str,size);
			str[size]=0;
			//
			char*a=strchr(str,'\r');
			if(a){
				ln_term[0]='\r';
				a=strrchr(str,'\n');
				if(a){ln_term[1]='\n';ln_term[2]=0;}
				else ln_term[1]=0;
			}
			ln_term_sze=strlen(ln_term);
			printpage(w,str);
			wmove(w,0,0);
		}
		close(fd);
	}
}
int main(int argc,char**argv){
	WINDOW*w1=initscr();
	int y=getmaxy(w1);
	WINDOW*w=newwin(y-1,getmaxx(w1),0,0);
	noecho();
	mousemask(ALL_MOUSE_EVENTS,NULL);
	if(argc==2){
		startpage(w,argv[1]);
		int n=strlen(argv[1]);
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
				s[p]=argv[1][p];
			}
			int f=open(s,O_WRONLY|O_CREAT|O_TRUNC,S_IRUSR|S_IWUSR);
			if(f!=-1){
				out_wr(f);
				close(f);
			}
			free(s);
		}
	}
	mvprintw(y-1,0,"h for help");
	wnoutrefresh(w1);move(0,0);
	loopin(w);
	endwin();
}
