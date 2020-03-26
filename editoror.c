//#include <string.h>
typedef unsigned int size_t;
extern size_t strlen(const char*);//3
//#include <fcntl.h>
//sys/types.h
typedef unsigned short mode_t;
extern int open(const char*,int,mode_t);
//asm-generic/fcntl.h
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
//#include<curses.h>
typedef void WINDOW;
extern WINDOW*initscr(void);
extern int endwin(void);
extern int wgetch(WINDOW*);//2
typedef unsigned mmask_t;
extern mmask_t mousemask(mmask_t,mmask_t*);
extern int noecho(void);
#define ALL_MOUSE_EVENTS 0xfffff
extern int move(int,int);//3
extern int getcury(const WINDOW*);//2
extern int getcurx(const WINDOW*);//2
extern int getmaxy(const WINDOW*);
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

#define NULL 0
typedef enum{true=1,false=0}bool;
typedef struct{
	char*str;
	char**t;
}inst;

#define mx_pt 100
int maxy;

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
	//if(y>maxy)return -1;
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
					if(m==0){move(y-1,x-1);}
					else if(m==64){
						y=getcury(w);
						if(y<maxy)move(y+1,getcurx(w));
					}
					else if(m==65){
						y=getcury(w);
						if(y>0)move(y-1,getcurx(w));
					}
				}
			}
		}
	}while(c!='q');
}
int main(int argc,char**argv){
	if(argc==2){
		int m=mx_pt;
		char*nm=argv[1];
		int s=strlen(nm);
		if(m==s)m--;
		char*ext=".s";
		int e=strlen(ext);
		m-=e;
		int n=m<s?m:s;
		char out[mx_pt+1];
		int i=0;
		for(;i<n;i++){
			out[i]=nm[i];
		}
		for(int j=0;j<e;j++){
			out[i]=ext[j];i++;
		}
		out[i]=0;
		inst instr[1];
		instr[0]=instrs[0];
		int f=open(out,O_WRONLY|O_CREAT|O_TRUNC,S_IRUSR|S_IWUSR);
		if(f!=-1){
			out_wr(f);
			close(f);
		}
	}
	WINDOW*w=initscr();
	maxy=getmaxy(w);
	noecho();
	mousemask(ALL_MOUSE_EVENTS,NULL);
	loopin(w);
	endwin();
}
