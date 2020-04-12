//#include <string.h>
typedef unsigned int size_t;
size_t strlen(const char*);//6
char*strchr(const char*,int);
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
int ungetch(int);
typedef unsigned int chtype;
chtype winch(WINDOW*);
typedef unsigned mmask_t;
mmask_t mousemask(mmask_t,mmask_t*);
int noecho(void);
#define ALL_MOUSE_EVENTS 0xfffff
int wmove(WINDOW*,int,int);//9
int move(int,int);//2
int getcury(const WINDOW*);//13
int getcurx(const WINDOW*);//6
int getmaxy(const WINDOW*);//5
int getmaxx(const WINDOW*);//5
WINDOW*newwin(int,int,int,int);
int delwin(WINDOW*);
int doupdate(void);
int wnoutrefresh(WINDOW*);//4
int mvprintw(int,int,const char*,...);//3
int mvwprintw(WINDOW*,int,int,const char*,...);//2
extern WINDOW*stdscr;//3
int werase(WINDOW*);//2
int clrtoeol(void);
int attrset(int);//2
int start_color(void);
int init_pair(short,short,short);
#define COLOR_BLACK 0
#define COLOR_WHITE 7
#define ERR -1
int COLOR_PAIR(int);
#define KEY_RESIZE 410
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
void*malloc(size_t);//4
void free(void*);//6
void*realloc(void*,size_t);//4
//#include<stdio.h>
int printf(const char*,...);//5
int getchar(void);

#define NULL 0
typedef char bool;
enum{false=0,true=1};
typedef struct{
	char*str;
	char**t;
}inst;

char ln_term[3]="\n";
int ln_term_sze;
char**rows;
int rows_tot=1;
int xtext;int ytext;
bool*x_right=NULL;
int*tabs=NULL;
#define tab_sz 6

char*textfile=NULL;
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
	int sz=strlen(a);
	x_right[r]=xtext<sz;
	if(!x_right[r])return;
	int max=getmaxx(w);
	int*ptr=&tabs[(1+max)*r];ptr[0]=0;
	int c=0;int cr=0;
	int i=xtext;int j=i;
	for(;i<sz&&c<max&&cr<max;i++){
		char z=a[i];
		if(z=='\t'){
			a[i]=0;mvwprintw(w,r,c,a+j);a[i]='\t';
			c+=i-j;
			ptr[ptr[0]+1]=c;ptr[0]++;
			j=i+1;
			c+=tab_sz;cr+=tab_sz-1;
		}else if(z<32||z>=127){
			a[i]='?';char aux=a[i+1];a[i+1]=0;
			mvwprintw(w,r,c,a+j);a[i+1]=aux;a[i]=z;
			c+=i-j+1;j=i+1;
		}
		cr++;
	}
	if(c<max){
		char e=a[i];
		a[i]=0;mvwprintw(w,r,c,a+j);a[i]=e;
	}
}
void printpage(WINDOW*w){
        int i=0;int maxy=getmaxy(w);
        int maxx=xtext+getmaxx(w);
        do{
		int j=ytext+i;
		char*str=rows[j];
		j++;
		char*str2=rows[j];
		bool last=j==rows_tot;
		if(!last)str2-=ln_term_sze;
		int sz=str2-str;
		if(sz>maxx)sz=maxx;
		char stor=str[sz];str[sz]=0;
                tab_grow(w,i,str);
		str[sz]=stor;
		if(last)break;
                i++;
        }while(i<maxy);
}
void refreshpage(WINDOW*w){
	werase(w);
	printpage(w);
}
void txmove(WINDOW*w,int x){
	int y=getcury(w);
	refreshpage(w);
	wmove(w,y,x);
}
void bmove(WINDOW*w,int r,int c,bool back){
	wmove(w,r,c);
	int chr=winch(w);
	if(chr==' '){
		int max=getmaxx(w);
		int*ptr=&tabs[(1+max)*r];
		int delim=ptr[0]+1;
		for(int i=1;i<delim;i++){
			int t=ptr[i];
			if((c<(t+tab_sz))&&(t<c)){
				if(back)wmove(w,r,t);
				else{
					c=t+tab_sz;
					if(c<max)wmove(w,r,c);
					else{
						int t1=ptr[1];
						int j=0;
						while(c>=max){
							xtext++;c--;
							if(j==t1){
								c-=tab_sz-1;
								break;
							}
							j++;
						}
						refreshpage(w);
						wmove(w,r,c);
					}
				}
				return;
			}
		}
	}
}
void amove(WINDOW*w,int r,int c){
	bmove(w,r,c,true);
}
void tmove(WINDOW*w,int y,bool right){
	int x=getcurx(w);
	if(right){
		if(ytext+1<rows_tot){ytext++;refreshpage(w);}
	}
	else if(ytext){ytext--;refreshpage(w);}
	amove(w,y,x);//print changed cursor
}
bool helpin(WINDOW*w){
	int c;
	do{
		c=getch();
		if(c==KEY_RESIZE)return true;
	}while(c!='q');
	int r=getcury(stdscr);
	for(int i=1;i<=r;i++){
		move(i,0);
		clrtoeol();
	}
	wnoutrefresh(stdscr);
	printpage(w);
	return false;
}
void helprows(char*s){
	mvprintw(getcury(stdscr)+1,0,s);
}
void printhelp(){
	attrset(COLOR_PAIR(1));
	mvprintw(getmaxy(stdscr)-1,0,"h for help");
	wnoutrefresh(stdscr);
	attrset(0);
}
void out_f(){
	int n=strlen(textfile);
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
			s[p]=textfile[p];
		}
		int f=open(s,O_WRONLY|O_CREAT|O_TRUNC,S_IRUSR|S_IWUSR);
		if(f!=-1){
			out_wr(f);
			close(f);
		}
		free(s);
	}
}
bool loopin(WINDOW*w){
	int c;
	do{
		c=wgetch(w);
		if(c=='\e'){
			c=ach(w);
			if(c=='['){
				c=ach(w);
				if(c=='<'){
					int x;int y;
					int m=mouse_test(w,&x,&y);
					if(m==0){amove(w,y-1,x-1);}
					else if(m==64){
						y=getcury(w);
						tmove(w,y,true);
					}
					else if(m==65){
						y=getcury(w);
						tmove(w,y,false);
					}
				}
				else if(c=='A'){
					int y=getcury(w);
					if(y>0)amove(w,y-1,getcurx(w));
					else tmove(w,y,false);
				}else if(c=='B'){
					int y=getcury(w);
					if(y+1<getmaxy(w))amove(w,y+1,getcurx(w));
					else tmove(w,y,true);
				}else if(c=='C'){
					int x=getcurx(w);
					if(x+1<getmaxx(w))bmove(w,getcury(w),x+1,false);
					else if(x_right[getcury(w)]){
						xtext++;
						txmove(w,x);
					}
				}else if(c=='D'){
					int x=getcurx(w);
					if(x>0)amove(w,getcury(w),x-1);
					else if(xtext>0){
						xtext--;
						txmove(w,x);
					}
				}else if(c=='H'){
					xtext=0;int y=getcury(w);
					refreshpage(w);
					wmove(w,y,0);
				}else if(c=='F'){
					int y=getcury(w);
					int r=ytext+y;
					xtext=0;
					if(r<rows_tot){
						xtext=rows[r+1]-rows[r];
						if(r+1<rows_tot)xtext-=ln_term_sze;
					}
					refreshpage(w);
					wmove(w,y,0);
				}
			}
		}
		else if(c=='h'){
			//if((getcurx(stdscr)|getcury(stdscr))==0){
			int cy=getcury(w);int cx=getcurx(w);
			werase(w);
			move(0,0);
			helprows("q is for quitting");
			helprows("c = compile file");
			helprows("mouse/touch press or v.scroll");
			wnoutrefresh(w);
			//}
			wnoutrefresh(stdscr);
			doupdate();
			if(helpin(w)){
				ungetch('h');
				return true;
			}
			wmove(w,cy,cx);
		}else if(c==KEY_RESIZE){
			return true;
		}
		else if(c=='c'){if(textfile)out_f();}
	}while(c!='q');
	return false;
}
int normalize(int*size,char**c){
	int ok=0;
	char*text_w=c[0];
	int sz=strlen(text_w);
	char*norm=(char*)malloc(2*sz+1);//-1 ok but,when sz=0,not ok
	if(norm){
		int j=0;
		for(int i=0;i<sz;i++){
			char a=text_w[i];
			if(a=='\n'){
				rows_tot++;
				if(ln_term_sze==2){
					norm[j]='\r';j++;ok=-1;
				}
				else if(ln_term[0]=='\r'){a='\r';ok=-1;}
			}else if(a=='\r'){
				rows_tot++;
				if(ln_term_sze==2){
					if(((i+1)<sz)&&text_w[i+1]=='\n'){
						norm[j]=a;j++;i++;
						a='\n';}
					else{norm[j]=a;j++;a='\n';ok=-1;}
				}
				else if(ln_term[0]=='\n'){a='\n';ok=-1;}
			}
			norm[j]=a;j++;
		}
		norm[j]=0;size[0]=j;
		free(text_w);c[0]=norm;
		if(!ok)ok=1;
	}
	return ok;
}
bool rows_init(char*a){
	char**m=(char**)realloc(rows,(rows_tot+1)*sizeof(char*));
	if(m){
		rows=m;
		for(int i=0;i<rows_tot;i++){
			rows[i]=a;
			a=strchr(a,ln_term[0])+ln_term_sze;
		}
		return true;
	}
	return false;
}
int startpage(char*f,char**c){
	int ok=1;
	int fd=open(f,O_RDONLY);
	if(fd!=-1){
		int size=lseek(fd,0,SEEK_END);
		char*tx=realloc(c[0],size+1);
		if(tx){
			c[0]=tx;
			lseek(fd,0,SEEK_SET);
			read(fd,tx,size);
			tx[size]=0;
			//
			for(int i=size-1;i>-1;i--){
				if(tx[i]=='\n'){
					if(i&&tx[i-1]=='\r'){
						ln_term[0]='\r';
						ln_term[1]='\n';
						ln_term[2]=0;
						break;
					}
					else break;
				}else if(tx[i]=='\r'){
					ln_term[0]='\r';
					break;
				}
			}
			ln_term_sze=strlen(ln_term);
			int sz;
			ok=normalize(&sz,c);
			if(ok){
				tx=c[0];
				if(rows_init(tx))rows[rows_tot]=&tx[sz];
				else ok=0;
			}
		}
		close(fd);
	}
	return ok;
}
int main(int argc,char**argv){
	int ret=0;
	char*text_w=(char*)malloc(1);
	if(!text_w)return ret;
	text_w[0]=0;
	rows=(char**)malloc(2*sizeof(char*));
	if(rows){
		rows[0]=text_w;rows[1]=text_w;
		int ok=1;
		if(argc==2){
			ok=startpage(argv[1],&text_w);
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
				if(ok)textfile=argv[1];
			}
		}
		if(ok){
			WINDOW*w1=initscr();
			noecho();
			mousemask(ALL_MOUSE_EVENTS,NULL);
			start_color();
			if(init_pair(1,COLOR_BLACK,COLOR_WHITE)!=ERR){
				bool loops=false;
				do{
					int r=getmaxy(w1)-1;
					char*a=realloc(x_right,r);
					if(!a)break;
					x_right=a;
					int c=getmaxx(w1);
					a=realloc(tabs,(1+c)*r*sizeof(int));
					if(!a)break;
					tabs=(int*)a;
					WINDOW*w=newwin(r,c,0,0);
					if(w){
						xtext=0;ytext=0;
						printpage(w);
						wmove(w,0,0);
						printhelp();
						loops=loopin(w);
						delwin(w);
					}else break;
				}while(loops);
				if(x_right){
					free(x_right);
					if(tabs)free(tabs);
				}
			}
			endwin();
		}
		free(rows);
	}
	free(text_w);
	return ret;
}
