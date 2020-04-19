typedef char bool;

//#include <string.h>
typedef unsigned int size_t;
size_t strlen(const char*);//8
char*strchr(const char*,int);
int strcmp(const char*,const char*);//6
void*memcpy(void*,const void*,size_t);
//#include <fcntl.h>
int open(const char*,int,...);//4
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
int close(int);//3
typedef int ssize_t;
ssize_t write(int,const void*,size_t);//3
typedef long off_t;
off_t lseek(int,off_t,int);//2
ssize_t read(int,void*,size_t);
//#include <bits/seek_constants.h>
#define SEEK_SET 0
#define SEEK_END 2
//#include<curses.h>
typedef void WINDOW;
WINDOW*initscr(void);
int endwin(void);
int getch(void);
int wgetch(WINDOW*);//2
int ungetch(int);
typedef unsigned int chtype;
chtype winch(WINDOW*);
typedef unsigned mmask_t;
mmask_t mousemask(mmask_t,mmask_t*);
int noecho(void);
#define ALL_MOUSE_EVENTS 0xFffFFff
int move(int,int);//6
int wmove(WINDOW*,int,int);//12
int getcury(const WINDOW*);//19
int getcurx(const WINDOW*);//12
int getmaxy(const WINDOW*);//10
int getmaxx(const WINDOW*);//6
WINDOW*newwin(int,int,int,int);
int delwin(WINDOW*);
int doupdate(void);
int wnoutrefresh(WINDOW*);//4
int addstr(const char*);//3
int mvaddstr(int,int,const char*);
int mvwaddstr(WINDOW*,int,int,const char*);//3
extern WINDOW*stdscr;//12
int werase(WINDOW*);//2
int clrtoeol(void);
int attrset(int);//2
int start_color(void);
int init_pair(short,short,short);
#define COLOR_BLACK 0
#define COLOR_WHITE 7
#define ERR -1
int COLOR_PAIR(int);
int keypad(WINDOW*,bool);//2
typedef unsigned int mmask_t;
typedef struct
{
    short id;
    int x,y,z;
    mmask_t bstate;
}
MEVENT;
#define OK 0
#define BUTTON1_CLICKED 0x4
#define BUTTON5_PRESSED 0x200000
#define BUTTON4_PRESSED 0x10000
#define KEY_UP 0403
#define KEY_DOWN 0402
#define KEY_LEFT 0404
#define KEY_RIGHT 0405
#define KEY_HOME 0406
#define KEY_END 0550
#define KEY_PPAGE 0523
#define KEY_NPAGE 0522
#define KEY_MOUSE 0631
#define KEY_RESIZE 0632
int getmouse(MEVENT*);
const char*keyname(int);
//#include<poll.h>
/*typedef unsigned int nfds_t;
struct pollfd{
	int fd;
	short events;
	short revents;
};
int poll(struct pollfd[],nfds_t,int);
#define POLLIN 0x0001
struct pollfd stdinfd={0,POLLIN,0};*/
//#include <stdlib.h>
void*malloc(size_t);//4
void free(void*);//7
void*realloc(void*,size_t);//5
char*getenv(const char*);
//#include<stdio.h>
int puts(const char*);//5
int sprintf(char*,const char*,...);
int getchar(void);

#define NULL 0
enum{false=0,true=1};

char ln_term[3]="\n";
int ln_term_sze;
char**rows;
int rows_tot=1;
int xtext;int ytext;
bool*x_right=NULL;
int*tabs=NULL;
#define tab_sz 6
int yhelp;
bool helpend;
char helptext[]="\nq is for quitting"
"\narrows,home/end,page up/down"
"\nb = compile file"
"\nmouse/touch press or v.scroll"
"\nalt arrows,ctrl home/end"
"\nv = visual mode"
"\n    c = copy";
void*cutbuf=NULL;
int cutbuf_sz=0;

typedef struct{
	char*str;
	char**t;
}inst;
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
/*int ach(WINDOW*w){
	if(poll(&stdinfd,1,0)<1)return 0;
	return wget ch(w);
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
	if(y[0]>get maxy(w))return -1;
	return a;
}*/
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
			a[i]=0;mvwaddstr(w,r,c,a+j);a[i]='\t';
			c+=i-j;
			ptr[ptr[0]+1]=c;ptr[0]++;
			j=i+1;
			c+=tab_sz;cr+=tab_sz-1;
		}else if(z<32||z>=127){
			a[i]='?';char aux=a[i+1];a[i+1]=0;
			mvwaddstr(w,r,c,a+j);a[i+1]=aux;a[i]=z;
			c+=i-j+1;j=i+1;
		}
		cr++;
	}
	if(c<max){
		char e=a[i];
		a[i]=0;mvwaddstr(w,r,c,a+j);a[i]=e;
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
void helpclear(){
	int r=getcury(stdscr);
	for(int i=0;i<=r;i++){
		move(i,0);
		clrtoeol();
	}
	move(getmaxy(stdscr)-2,0);
	addstr("   ");
}
void printinverted(char*s){
	attrset(COLOR_PAIR(1));
	addstr(s);
	//attr set here,cause,print"   "
	attrset(0);
}
void helpposition(){
	int x=getcurx(stdscr);int y=getcury(stdscr);
	move(getmaxy(stdscr)-2,0);
	if(helpend)printinverted("BOT");
	else if(!yhelp)printinverted("TOP");
	else addstr(":  ");
	move(y,x);
}
void helpshow(int n){
	yhelp=n;
	move(0,0);	
	int i=0;int j=0;int y=0;
	do{
		helpend=helptext[i]==0;
		if(helptext[i]=='\n'||helpend){
			if(n)n--;
			else{
				helptext[i]=0;
				mvaddstr(y,0,&helptext[j]);
				if(!helpend)helptext[i]='\n';
				y=getcury(stdscr)+1;
				if(getmaxy(stdscr)-3<y)break;
			}
		 	j=i+1;
		}
		i++;
	}while(!helpend);
	helpposition();
}
void hmove(int n){
	if(helpend&&(n>0))return;
	n+=yhelp;
	if(n<0)return;
	helpclear();
	helpshow(n);
}
bool helpin(WINDOW*w){
	int c;
	do{
		c=getch();
		if(c==KEY_DOWN)hmove(1);
		else if(c==KEY_UP)hmove(-1);
		else if(c==KEY_RESIZE)return true;
	}while(c!='q');
	helpclear();
	wnoutrefresh(stdscr);
	printpage(w);
	return false;
}
void printhelp(){
	move(getmaxy(stdscr)-1,0);
	printinverted("h for help");
	wnoutrefresh(stdscr);
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
void sumove(WINDOW*w,int y){tmove(w,y,false);}
void sdmove(WINDOW*w,int y){tmove(w,y,true);}
void slmove(WINDOW*w,int x,bool notabs){
	int y=getcury(w);
	if(xtext>0){
		xtext--;
		refreshpage(w);
		if(notabs)wmove(w,y,x);
		else amove(w,y,x);
	}
}
void srmove(WINDOW*w,int x){
	int y=getcury(w);
	if(x_right[y]){
		xtext++;
		refreshpage(w);
		amove(w,y,x);
	}
}
int end(WINDOW*w,int r){
	char*s=rows[r+1];char*b=rows[r];
	if(r+1<rows_tot)s-=ln_term_sze;
	int n=getmaxx(w)-1;int m=0;
	while(s>b&&(s-b!=xtext)){
		s--;
		m+=s[0]=='\t'?tab_sz:1;
		if(m>=n)break;
	}
	xtext=s-b;
	return m;
}
//-1 different key,0move key,1resize
int movment(int c,WINDOW*w){
	if(c==KEY_MOUSE){
		MEVENT e;
		int a=getmouse(&e);
		if(a==OK){
			if(e.bstate&BUTTON1_CLICKED)amove(w,e.y-1,e.x-1);
			else if(e.bstate&BUTTON5_PRESSED)tmove(w,getcury(w),false);
			else if(e.bstate&BUTTON4_PRESSED)tmove(w,getcury(w),true);
		}
	}else if(c==KEY_UP){
		int y=getcury(w);
		if(y>0)amove(w,y-1,getcurx(w));
		else sumove(w,y);
	}else if(c==KEY_DOWN){
		int y=getcury(w);
		if(y+1<getmaxy(w))amove(w,y+1,getcurx(w));
		else sdmove(w,y);
	}else if(c==KEY_LEFT){
		int x=getcurx(w);
		if(x>0)amove(w,getcury(w),x-1);
		else slmove(w,x,true);
	}else if(c==KEY_RIGHT){
		int x=getcurx(w);
		if(x+1<getmaxx(w))bmove(w,getcury(w),x+1,false);
		else srmove(w,x);
	}else if(c==KEY_HOME){
		xtext=0;int y=getcury(w);
		refreshpage(w);
		wmove(w,y,0);
	}else if(c==KEY_END){
		int y=getcury(w);
		int r=ytext+y;int x;
		if(r<rows_tot)x=end(w,r);
		else{xtext=0;x=0;}
		refreshpage(w);
		wmove(w,y,x);
	}else if(c==KEY_PPAGE){
		int y=getcury(w);int x=getcurx(w);
		ytext-=getmaxy(w);
		if(ytext<0)ytext=0;
		refreshpage(w);
		amove(w,y,x);
	}else if(c==KEY_NPAGE){
		int y=getcury(w);int x=getcurx(w);
		ytext+=getmaxy(w);
		if(ytext+1>rows_tot)ytext=rows_tot-1;
		refreshpage(w);
		wmove(w,y,x);
	}else if(c==KEY_RESIZE){
		return 1;
	}
	else{
		const char*s=keyname(c);
		if(!strcmp(s,"kUP3"))sumove(w,getcury(w));
		else if(!strcmp(s,"kDN3"))sdmove(w,getcury(w));
		else if(!strcmp(s,"kLFT3"))slmove(w,getcurx(w),false);
		else if(!strcmp(s,"kRIT3"))srmove(w,getcurx(w));
		else if(!strcmp(s,"kHOM5")){
			ytext=0;xtext=0;
			refreshpage(w);
			wmove(w,0,0);
		}else if(!strcmp(s,"kEND5")){
			int a=rows_tot-1;
			ytext=a-getmaxy(w)+1;
			if(ytext<0)ytext=0;
			int y=a-ytext;
			int x=end(w,a);
			refreshpage(w);
			//moved by curses, but no add str for line breaks
			wmove(w,y,x);
		}else return -1;
	}
	return 0;
}
void fixmembuf(int*y,int*x){
	if(y[0]>rows_tot-1)y[0]=rows_tot-1;
	int r=y[0];
	int sz=rows[r+1]-rows[r];
	if(r<rows_tot-1)sz-=ln_term_sze;
	if(x[0]>sz)x[0]=sz;
}
int set2membuf(int yesel,int xesel){
	int sz=rows[yesel+1]-rows[yesel];
	bool a=yesel<rows_tot-1;
	if(a)sz-=ln_term_sze;
	if(xesel==sz){
		if(a)return ln_term_sze;
		return 0;
	}
	return 1;
}
void writemembuf(int ybsel,int xbsel,int yesel,int xesel){
	fixmembuf(&ybsel,&xbsel);
	fixmembuf(&yesel,&xesel);
	xesel+=set2membuf(yesel,xesel);
	char*b=rows[ybsel]+xbsel;
	char*e=rows[yesel]+xesel;
	int sz=e-b;
	if(cutbuf_sz<sz){
		void*v=realloc(cutbuf,sz);
		if(!v)return;
		cutbuf=v;cutbuf_sz=sz;
	}
	memcpy(cutbuf,b,sz);
}
void set1membuf(int y,int x,int*yb,int*xb,int*ye,int*xe){
	bool a=false;
	if(y<yb[0])a=true;
	else if(y>yb[0]){}
	else if(x<xb[0])a=true;
	if(a){ye[0]=yb[0];xe[0]=xb[0];
		yb[0]=y;xb[0]=x;}
	else{ye[0]=y;xe[0]=x;}
}
bool loopin(WINDOW*w){
	int c;
	do{
		c=wgetch(w);
		int a=movment(c,w);
		if(a>0)return true;
		if(a){
			if(c=='b'){if(textfile)out_f();}
			else if(c=='v'){
				int ybsel=ytext+getcury(w);
				int xbsel=xtext+getcurx(w);
				int yesel=ybsel;int xesel=xbsel;
				int z;
				do{
					int b=wgetch(w);
					z=movment(b,w);
					if(z>0)return true;
					else if(z){
						if(b=='c')writemembuf(ybsel,xbsel,yesel,xesel);
					}else{
						int y=ytext+getcury(w);
						int x=xtext+getcurx(w);
						set1membuf(y,x,&ybsel,&xbsel,&yesel,&xesel);
					}
				}while(!z);
			}
			else if(c=='h'){
				int cy=getcury(w);int cx=getcurx(w);
				werase(w);
				helpshow(0);
				wnoutrefresh(w);
				wnoutrefresh(stdscr);
				doupdate();
				if(helpin(w)){
					ungetch('h');
					return true;
				}
				wmove(w,cy,cx);
			}
		}
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
int setfilebuf(char*s,char*cutbuf_file){
	size_t sz=strlen(s);int i=sz-1;
	while(i>=0){
		char a=s[i];
		if(a=='/'||a=='\\')break;
		i--;
	}
	char*h=getenv("HOME");
	if(!h)return 0;
	size_t l=strlen(h);
	if(!l)return 0;
	i++;sz-=i;
	if(l+sz+7>128)return 0;
	sprintf(cutbuf_file,"%s/.%sinfo",h,&s[i]);
	return l-1;
}
void writefilebuf(char*cutbuf_file,int off){
	if(cutbuf_file[0]){
		int f=open(cutbuf_file,O_WRONLY|O_TRUNC);
		if(f==-1){
			cutbuf_file[off]='.';
			f=open(cutbuf_file+off,O_WRONLY|O_TRUNC);
		}
		if(f!=-1){
			write(f,cutbuf,cutbuf_sz);
			close(f);
		}
	}
	if(cutbuf)free(cutbuf);
}
int main(int argc,char**argv){
	char*text_w=(char*)malloc(1);
	if(text_w){
		text_w[0]=0;
		rows=(char**)malloc(2*sizeof(char*));
		if(rows){
			rows[0]=text_w;rows[1]=text_w;
			int ok=1;
			if(argc==2){
				ok=startpage(argv[1],&text_w);
				if(ok){
					if(ok<1){
						puts("Normalize line endings to ");
						if(ln_term_sze==2)puts("\\r\\n");
						else if(ln_term[0]=='\n')puts("\\n");
						else puts("\\r");
						puts("? n=no, default=yes\n");
						int c=getchar();
						if(c=='n')ok=0;
					}
					if(ok)textfile=argv[1];
				}
			}
			if(ok){
				WINDOW*w1=initscr();
				keypad(stdscr,true);	
				mousemask(ALL_MOUSE_EVENTS,NULL);
				start_color();
				noecho();
				if(init_pair(1,COLOR_BLACK,COLOR_WHITE)!=ERR){
					char cutbuf_file[128];
					cutbuf_file[0]=0;
					int off=setfilebuf(argv[0],cutbuf_file);
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
							keypad(w,true);
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
						if(tabs){
							free(tabs);
							writefilebuf(cutbuf_file,off);
						}
					}
				}
				endwin();
			}
			free(rows);
		}
		free(text_w);
	}
	return 0;
}
