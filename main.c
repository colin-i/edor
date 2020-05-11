#include"src/main0.h"
//strlen,2;open,4;close,3;write
#include"src/main.h"
//malloc,10;free,11;realloc,6
#include"src/main2.h"
//move,6;getch;getmaxy,14;stdscr,14
//keyname,2;getcurx,16;strcmp,12

//#include <string.h>
void*memcpy(void*,const void*,size_t);//16
//sys/types.h
typedef unsigned short mode_t;
//asm-generic/fcntl.h
#define O_RDONLY 00000000
//#include <unistd.h>
typedef long off_t;
off_t lseek(int,off_t,int);//4
ssize_t read(int,void*,size_t);//2
//#include <bits/seek_constants.h>
#define SEEK_SET 0
#define SEEK_END 2
//#include<curses.h>
WINDOW*initscr(void);
int endwin(void);
int wgetch(WINDOW*);//2
int ungetch(int);
chtype winch(WINDOW*);
int winnstr(WINDOW*,char*,int);//2
typedef unsigned mmask_t;
mmask_t mousemask(mmask_t,mmask_t*);
int noecho(void);
int cbreak(void);
int nonl(void);
#define ALL_MOUSE_EVENTS 0xFffFFff
int wmove(WINDOW*,int,int);//21
int getcury(const WINDOW*);//22
int getmaxx(const WINDOW*);//14
WINDOW*newwin(int,int,int,int);
int delwin(WINDOW*);
int doupdate(void);//2
int wnoutrefresh(WINDOW*);//6
int waddch(WINDOW*,const chtype);//2
int mvaddch(int,int,const chtype);
int addstr(const char*);//3
int waddstr(WINDOW*,const char*);//4
int waddnstr(WINDOW*,const char*,int);//2
int mvaddstr(int,int,const char*);
int werase(WINDOW*);//2
int clrtoeol(void);//2
int wclrtoeol(WINDOW*);
int attrset(int);//3
int wattrset(WINDOW*,int);
int start_color(void);
int init_pair(short,short,short);//2
#define COLOR_BLACK 0
#define COLOR_CYAN 6
#define COLOR_WHITE 7
#define ERR -1
int COLOR_PAIR(int);//2
int keypad(WINDOW*,bool);//2
typedef unsigned int mmask_t;
typedef struct
{
	short id;// __attribute__((aligned(4)));
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
#define KEY_HOME 0406
#define KEY_DC 0512
#define KEY_END 0550
#define KEY_PPAGE 0523
#define KEY_NPAGE 0522
#define KEY_MOUSE 0631
int getmouse(MEVENT*);
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
char*getenv(const char*);
//#include<stdio.h>
int puts(const char*);
int sprintf(char*,const char*,...);
int getchar(void);

char ln_term[3]="\n";
size_t ln_term_sz=1;

static char*textfile=NULL;
static row*rows=NULL;
static size_t rows_tot=1;
static size_t rows_spc=1;
static size_t xtext;
static size_t ytext;
static bool*x_right=NULL;
static int*tabs=NULL;
static int tabs_rsz;
#define tab_sz 6
static int yhelp;
static bool helpend;
static char helptext[]="INPUT"
"\nhelp"
"\n    q(uit),up/down"
"\narrows(+-alt),home/end(+-ctrl),page up/down"
"\nmouse/touch press or v.scroll"
"\nCtrl+v = visual mode"
"\n    c = copy"
"\n    d = delete"
"\nCtrl+p = paste"
"\nCtrl+s = save file"
"\ncommand mode"
"\n    left/right,ctrl+q"
"\nCtrl+b = build file"
"\nCtrl+q = quit";
static bool visual_bool=false;
static char*cutbuf=NULL;
static size_t cutbuf_sz=0;
static char*mapsel=NULL;
static size_t cutbuf_spc=0;
static size_t cutbuf_r=1;
static char*text_init_b=NULL;
static char*text_init_e;
#define row_pad 0xF

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
bool no_char(char z){return z<32||z>=127;}
static void tab_grow(WINDOW*w,int r,char*a,size_t sz,int*ptr){
	x_right[r]=xtext<sz;
	if(!x_right[r])return;
	int c=0;int cr=0;
	int max=getmaxx(w);
	size_t i=xtext;size_t j=i;
	wmove(w,r,0);
	for(;i<sz&&cr<max;i++){
		char z=a[i];
		if(z=='\t'){
			int n=(int)(i-j);
			waddnstr(w,a+j,n);
			c+=n;
			ptr[ptr[0]+1]=c;ptr[0]++;
			j=i+1;
			c+=tab_sz;cr+=tab_sz-1;
			wmove(w,r,c);
		}else if(no_char(z)){
			a[i]='?';char aux=a[i+1];a[i+1]=0;
			waddstr(w,a+j);a[i+1]=aux;a[i]=z;
			c+=i-j+1;j=i+1;
		}
		cr++;
	}
	if(c<max){
		char e=a[i];
		a[i]=0;waddstr(w,a+j);a[i]=e;
	}
}
static void printpage(WINDOW*w){
	int i=0;int maxy=getmaxy(w);
	size_t maxx=xtext+(size_t)getmaxx(w);
	do{
		size_t j=ytext+(size_t)i;
		int*ptr=&tabs[tabs_rsz*i];ptr[0]=0;
		if(j<rows_tot){
			char*str=rows[j].data;
			size_t sz=rows[j].sz;
			if(sz>maxx)sz=maxx;
			char stor=str[sz];str[sz]=0;
			tab_grow(w,i,str,sz,ptr);
			str[sz]=stor;
		}else x_right[i]=false;
		i++;
	}while(i<maxy);
}
static void refreshpage(WINDOW*w){
	werase(w);
	printpage(w);
}
static bool bmove(WINDOW*w,int r,int c,bool back){
	wmove(w,r,c);
	char chr=(char)winch(w);
	if(chr==' '){
		int*ptr=&tabs[tabs_rsz*r];
		int n=ptr[0];
		for(int i=1;i<=n;i++){
			int t=ptr[i];
			if((c<(t+tab_sz))&&(t<c)){
				if(back)wmove(w,r,t);
				else{
					c=t+tab_sz;
					int max=getmaxx(w);
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
						return false;
					}
				}
				return true;
			}
		}
	}
	return true;
}
static void amove(WINDOW*w,int r,int c){
	bmove(w,r,c,true);
}
static void tmove(WINDOW*w,int y,bool right){
	int x=getcurx(w);
	if(right){
		if(ytext+1<rows_tot){ytext++;refreshpage(w);}
	}
	else if(ytext){ytext--;refreshpage(w);}
	amove(w,y,x);//print changed cursor
}
static void helpclear(){
	int r=getcury(stdscr);
	for(int i=0;i<=r;i++){
		move(i,0);
		clrtoeol();
	}
	move(getmaxy(stdscr)-2,0);
	addstr("   ");
}
static void printinverted(char*s){
	attrset(COLOR_PAIR(1));
	addstr(s);
	//attr set here,cause,print"   "
	attrset(0);
}
static void helpposition(){
	int x=getcurx(stdscr);int y=getcury(stdscr);
	move(getmaxy(stdscr)-2,0);
	if(helpend)printinverted("BOT");
	else if(!yhelp)printinverted("TOP");
	else addstr(":  ");
	move(y,x);
}
static void helpshow(int n){
	yhelp=n;
	move(0,0);	
	int i=0;int j=0;int y=0;
	int max=getmaxx(stdscr);int c=0;
	do{
		helpend=helptext[i]==0;
		bool newl=helptext[i]=='\n';
		if(newl||helpend||c==max){
			if(n)n--;
			else{
				char aux=helptext[i];helptext[i]=0;
				mvaddstr(y,0,&helptext[j]);
				helptext[i]=aux;
				y++;
				if(getmaxy(stdscr)-3<y)break;
			}
			j=i;if(newl)j++;
			c=0;
		}else c++;
		i++;
	}while(!helpend);
	helpposition();
}
static void hmove(int n){
	if(helpend&&(n>0))return;
	n+=yhelp;
	if(n<0)return;
	helpclear();
	helpshow(n);
}
static bool helpin(WINDOW*w){
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
static void printhelp(){
	move(getmaxy(stdscr)-1,0);
	clrtoeol();//when resized one row up
	printinverted(bar_init());
	wnoutrefresh(stdscr);
}
static void sumove(WINDOW*w,int y){tmove(w,y,false);}
static void sdmove(WINDOW*w,int y){tmove(w,y,true);}
static void slmove(WINDOW*w,int x,bool notabs){
	int y=getcury(w);
	if(xtext>0){
		xtext--;
		refreshpage(w);
		if(notabs)wmove(w,y,x);
		else amove(w,y,x);
	}
}
static void srmove(WINDOW*w,int x,bool back){
	int y=getcury(w);
	if(x_right[y]){
		xtext++;
		refreshpage(w);
		bmove(w,y,x,back);
	}
}
static int end(WINDOW*w,size_t r){
	size_t sz=rows[r].sz;
	if(xtext>=sz){xtext=sz;return 0;}
	char*b=rows[r].data;
	char*s=b+sz;
	int n=getmaxx(w)-1;int m=0;
	do{
		s--;m+=s[0]=='\t'?tab_sz:1;
		if((size_t)(s-b)==xtext)break;
	}while(m<n);
	if(m>n){
		s++;m-=tab_sz;
	}
	xtext=(size_t)(s-b);
	return m;
}
//1resize,0diff key,-,1refreh,2press,3back,4forward
static int movment(int c,WINDOW*w){
	if(c==KEY_MOUSE){
		MEVENT e;
		int a=getmouse(&e);
		if(a==OK){
			if(e.bstate&BUTTON1_CLICKED){amove(w,e.y-1,e.x-1);return -2;}
			else if(e.bstate&BUTTON5_PRESSED)tmove(w,getcury(w),false);
			else if(e.bstate&BUTTON4_PRESSED)tmove(w,getcury(w),true);
		}
	}else if(c==KEY_UP){
		int y=getcury(w);
		if(y>0){amove(w,y-1,getcurx(w));return -3;}
		else sumove(w,y);
	}else if(c==KEY_DOWN){
		int y=getcury(w);
		if(y+1<getmaxy(w)){amove(w,y+1,getcurx(w));return -4;}
		else sdmove(w,y);
	}else if(c==KEY_LEFT){
		int x=getcurx(w);
		if(x>0){amove(w,getcury(w),x-1);return -3;}
		else slmove(w,x,true);
	}else if(c==KEY_RIGHT){
		int x=getcurx(w);
		if(x+1<getmaxx(w)){if(bmove(w,getcury(w),x+1,false))return -4;}
		else srmove(w,x,false);
	}else if(c==KEY_HOME){
		xtext=0;int y=getcury(w);
		refreshpage(w);
		wmove(w,y,0);
	}else if(c==KEY_END){
		int y=getcury(w);
		size_t r=ytext+(size_t)y;int x;
		if(r<rows_tot)x=end(w,r);
		else{xtext=0;x=0;}
		refreshpage(w);
		wmove(w,y,x);
	}else if(c==KEY_PPAGE){
		int y=getcury(w);int x=getcurx(w);
		size_t my=(size_t)getmaxy(w);
		ytext=my>ytext?0:ytext-my;
		refreshpage(w);
		amove(w,y,x);
	}else if(c==KEY_NPAGE){
		int y=getcury(w);int x=getcurx(w);
		ytext+=(size_t)getmaxy(w);
		if(ytext+1>rows_tot)ytext=rows_tot-1;
		refreshpage(w);
		amove(w,y,x);
	}else if(c==KEY_RESIZE){
		return 1;
	}
	else{
		const char*s=keyname(c);
		if(!strcmp(s,"kUP3"))sumove(w,getcury(w));
		else if(!strcmp(s,"kDN3"))sdmove(w,getcury(w));
		else if(!strcmp(s,"kLFT3"))slmove(w,getcurx(w),false);
		else if(!strcmp(s,"kRIT3"))srmove(w,getcurx(w),true);
		else if(!strcmp(s,"kHOM5")){
			ytext=0;xtext=0;
			refreshpage(w);
			wmove(w,0,0);
		}else if(!strcmp(s,"kEND5")){
			size_t max=(size_t)getmaxy(w);
			ytext=rows_tot<=max?0:rows_tot-max;
			size_t a=rows_tot-1;
			int y=(int)(a-ytext);
			int x=end(w,a);
			refreshpage(w);
			//moved by curses, but no add str for line breaks
			wmove(w,y,x);
		}else return 0;
	}
	return -1;
}
static void fixmembuf(size_t*y,size_t*x){
	bool last=y[0]>=rows_tot;
	if(last)y[0]=rows_tot-1;
	size_t r=y[0];
	size_t sz=rows[r].sz;
	if(last||x[0]>sz)x[0]=sz;
}
static size_t set2membuf(size_t yesel,size_t xesel){
	if(xesel==rows[yesel].sz){
		if(yesel<rows_tot-1)return ln_term_sz;
		return 0;
	}
	return 1;
}
static bool writemembuf(size_t ybsel,size_t xbsel,size_t yesel,size_t xesel){
	fixmembuf(&ybsel,&xbsel);
	fixmembuf(&yesel,&xesel);
	xesel+=set2membuf(yesel,xesel);
	row*b=&rows[ybsel];
	bool one=ybsel==yesel;size_t size;
	size_t sz1;
	if(one)size=xesel-xbsel;
	else{
		sz1=b->sz-xbsel;
		size=sz1+ln_term_sz+xesel;
		for(size_t i=ybsel+1;i<yesel;i++){
			size+=rows[i].sz+ln_term_sz;
		}
	}
	if(cutbuf_spc<size){
		void*v=realloc(cutbuf,size);
		if(!v)return false;
		cutbuf=v;cutbuf_spc=size;
	}
	bool inc_nl=rows[yesel].sz<xesel;
	if(one){
		size_t n=xesel-xbsel;
		if(inc_nl)n-=ln_term_sz;
		memcpy(cutbuf,b->data+xbsel,n);
		if(inc_nl)memcpy(cutbuf+n,ln_term,ln_term_sz);
	}
	else{
		memcpy(cutbuf,b->data+xbsel,sz1);
		memcpy(cutbuf+sz1,ln_term,ln_term_sz);
		size_t sz=sz1+ln_term_sz;
		for(size_t i=ybsel+1;i<yesel;i++){
			size_t s=rows[i].sz;
			memcpy(cutbuf+sz,rows[i].data,s);
			sz+=s;
			memcpy(cutbuf+sz,ln_term,ln_term_sz);
			sz+=ln_term_sz;
		}
		size_t n=xesel;
		if(inc_nl)n-=ln_term_sz;
		memcpy(cutbuf+sz,rows[yesel].data,n);
		if(inc_nl)memcpy(cutbuf+sz+n,ln_term,ln_term_sz);
	}
	cutbuf_sz=size;cutbuf_r=yesel-ybsel+1;
	if(inc_nl)cutbuf_r++;
	return true;
}
static void set1membuf(size_t y,size_t x,bool*orig,size_t*yb,size_t*xb,size_t*ye,size_t*xe){
	if(orig[0]){
		if(y<yb[0]){
			ye[0]=yb[0];xe[0]=xb[0];xb[0]=x;orig[0]=false;
			yb[0]=y;
		}
		else if(y>yb[0]){
			ye[0]=y;xe[0]=x;
		}
		else if(x<xb[0]){
			ye[0]=yb[0];xe[0]=xb[0];xb[0]=x;orig[0]=false;
		}
		else{
			ye[0]=y;xe[0]=x;
		}
	}else{
		if(ye[0]<y){
			yb[0]=ye[0];xb[0]=xe[0];xe[0]=x;orig[0]=true;
			ye[0]=y;
		}
		else if(ye[0]>y){
			yb[0]=y;xb[0]=x;
		}
		else if(xe[0]<x){
			yb[0]=ye[0];xb[0]=xe[0];xe[0]=x;orig[0]=true;
		}
		else{
			yb[0]=y;xb[0]=x;
		}
	}
}
static size_t c_to_xc(int c,int r){
	int*p=&tabs[tabs_rsz*r];
	int n=p[0];int x=c;
	for(int i=0;i<n;i++){
		p++;
		//cright can be in tab
		if((p[0]+(tab_sz-1))<c)x-=tab_sz-1;
		else break;
	}
	return (size_t)x;
}
static int xc_to_c(int col,int r){
	int*p=&tabs[tabs_rsz*r];
	int n=p[0];
	for(int i=0;i<n;i++){
		p++;
		if(p[0]<col)col+=tab_sz-1;
		else break;
	}
	return col;
}
static int mid(int r,int max){
	size_t y=ytext+(size_t)r;
	if(y<rows_tot){
		size_t sz=rows[y].sz;
		if(sz<xtext)return 0;
		sz-=xtext;
		int w=tabs[r*tabs_rsz]*(tab_sz-1)+(int)sz;
		if(w<max){
			return w;
		}
		return max;
	}
	return 0;
}
static void printpart(WINDOW*w,int p,int n){
	wattrset(w,COLOR_PAIR(p));
	winnstr(w,mapsel,n);
	waddstr(w,mapsel);
}
static void printrow(WINDOW*w,int r,int b,int e){
	int m=mid(r,getmaxx(w));
	if(m<=b){
		printpart(w,2,e-b);
		return;
	}
	if(e<=m){
		printpart(w,1,e-b);
		return;
	}
	printpart(w,1,m-b);
	printpart(w,2,e-m);
}
static void printsel(WINDOW*w,size_t ybsel,size_t xbsel,size_t yesel,size_t xesel){
	int wd=getmaxx(w);
	int cright=wd-1;
	int rb;int cb;
	if(ybsel<ytext){
		rb=0;cb=0;}
	else{
		rb=(int)(ybsel-ytext);
		if(xbsel<=xtext)cb=0;
		else if(xbsel<=xtext+c_to_xc(cright,rb))cb=xc_to_c((int)(xbsel-xtext),rb);
		else{rb++;cb=0;}
	}
	int re;int ce;
	int rdown=getmaxy(w)-1;
	size_t ydown=ytext+(size_t)rdown;
	if(yesel>ydown){
		re=rdown;ce=cright;}
	else{
		re=(int)(yesel-ytext);
		if(xtext+c_to_xc(cright,re)<=xesel)ce=cright;
		else if(xtext<=xesel)ce=xc_to_c((int)(xesel-xtext),re);
		else{re--;ce=cright;}
	}
	int*t=&tabs[re*tabs_rsz];
	for(int i=t[0];i>0;i--){
		if(t[i]<=ce){
			if(t[i]==ce){
				ce+=tab_sz-1;
				if(ce>=wd)ce=wd-1;
			}
			break;
		}
	}
	wmove(w,rb,cb);
	ce++;
	if(rb==re){
		printrow(w,rb,cb,ce);
	}else{
		printrow(w,rb,cb,wd);
		for(int r=rb+1;r<re;r++){
			int m=mid(r,wd);
			printpart(w,1,m);
			printpart(w,2,wd-m);
		}
		printrow(w,re,0,ce);
	}
	wattrset(w,0);
}
static bool testrefresh(int z,bool orig){
	if(z>-2)return false;
	return z==-2||(z==-3&&orig)||!orig;
}
static void visual(char a){
	mvaddch(getmaxy(stdscr)-1,getmaxx(stdscr)-1,a);
	wnoutrefresh(stdscr);
}
static void pasted(size_t r,size_t x,WINDOW*w){
	size_t rws=cutbuf_r-1;
	r+=rws;size_t maxy=(size_t)getmaxy(w);
	if(maxy<=r){
		ytext+=r-maxy+1;
		r=maxy-1;
	}
	//
	int c=0;
	if(x<=xtext)xtext=x;
	else{
		char*d=rows[ytext+r].data;
		char*b=d+xtext;
		char*s=d+x;int maxx=getmaxx(w)-1;
		do{
			s--;c+=s[0]=='\t'?tab_sz:1;
			if(b==s)break;
		}while(c<maxx);
		if(c>maxx){s++;c-=tab_sz;}
		xtext=(size_t)(s-d);
	}
	refreshpage(w);wmove(w,(int)r,c);
}
static bool rows_expand(size_t n){
	size_t rowssize=rows_tot+n;
	if(rowssize>rows_spc){
		row*m=(row*)realloc(rows,rowssize*sizeof(row));
		if(!m)return true;
		rows=m;rows_spc=rowssize;
	}
	return false;
}
static void text_free(size_t b,size_t e){
	for(size_t i=b;i<e;i++){
		char*d=rows[i].data;
		if(d<text_init_b||text_init_e<=d)free(d);
	}
}
static size_t row_pad_sz(size_t sz){
	size_t dif=sz&row_pad;
	if(dif)return sz+((dif^row_pad)+1);
	return sz;
}
static bool mal_spc_rea(row*rw,size_t l,size_t c,size_t r,char*mid){
	char*src=rw->data;char*dst;
	size_t sz=l+c+r;
	if(sz>rw->spc){
		size_t size=row_pad_sz(sz);
		if(text_init_b<=src&&src<text_init_e){
			dst=(char*)malloc(size);
			if(!dst)return true;
			memcpy(dst,src,l);
		}else{
			dst=realloc(src,size);
			if(!dst)return true;
			src=dst;
		}
		rw->data=dst;
		rw->spc=size;
	}else dst=src;
	size_t j=l+c;size_t k=l+r;size_t i=j+r;
	while(j<i){
		i--;k--;dst[i]=src[k];
	}
	dst+=l;
	while(c>0){
		dst[0]=mid[0];
		dst++;mid++;c--;
	}
	rw->sz=sz;
	return false;
}
static void deleted(size_t ybsel,size_t xbsel,int*r,int*c,WINDOW*w){
	if(ybsel<ytext){ytext=ybsel;r[0]=0;}
	else r[0]=(int)(ybsel-ytext);
	if(xbsel<=xtext){
		xtext=xbsel;c[0]=0;
		return;
	}
	char*d=rows[ybsel].data;
	int cl=0;
	int max=getmaxx(w);
	size_t x=xbsel;
	while(x>xtext){
		x--;cl+=d[x]=='\t'?tab_sz:1;
		if(cl>=max){
			if(cl>max){
				cl-=tab_sz;
				x++;
			}
			break;
		}
	}
	xtext=x;
	c[0]=cl;
}
static void row_del(size_t a,size_t b){
	size_t c=b+1;
	text_free(a,c);
	row*j=&rows[a];
	for(size_t i=c;i<rows_tot;i++){
		memcpy(j,&rows[i],sizeof(row));
		j++;
	}
	rows_tot-=c-a;
}
static void delete(size_t ybsel,size_t xbsel,size_t yesel,size_t xesel,int*rw,int*cl,WINDOW*w){
	fixmembuf(&ybsel,&xbsel);
	fixmembuf(&yesel,&xesel);
	xesel+=set2membuf(yesel,xesel);
	//
	row*r1=&rows[ybsel];
	if(xesel>rows[yesel].sz){yesel++;xesel=0;}
	if(ybsel==yesel){
		size_t sz=r1->sz;
		size_t dif=xesel-xbsel;
		char*d=rows[ybsel].data;
		for(size_t i=xesel;i<sz;i++){
			d[i-dif]=d[i];
		}
		r1->sz-=dif;
	}else{
		size_t c=rows[yesel].sz-xesel;
		if(mal_spc_rea(r1,xbsel,c,0,rows[yesel].data+xesel))return;
		row_del(ybsel+1,yesel);
	}
	deleted(ybsel,xbsel,rw,cl,w);
}
static char*memtrm(char*a){
	while(a[0]!=ln_term[0])a++;
	return a;
}
static void rows_insert(row*d,size_t sz,size_t off){
	size_t a=rows_tot-1;
	rows_tot+=sz;
	while(off<=a){
		memcpy(&rows[a+sz],&rows[a],sizeof(row));
		a--;
	}
	memcpy(rows+off,d,sz*sizeof(row));
}
static size_t pasting(row*d,int r,int c,WINDOW*w){
	size_t y=ytext+(size_t)r;
	size_t x=xtext+c_to_xc(c,r);
	fixmembuf(&y,&x);
	//
	bool one=cutbuf_r==1;
	size_t szc;size_t sz1r;size_t l;
	size_t szr=rows[y].sz-x;
	size_t max=cutbuf_r-1;
	if(one){
		szc=cutbuf_sz;sz1r=szr;
	}
	else{
		char*a=memtrm(cutbuf)+ln_term_sz;
		sz1r=0;
		size_t sz=(size_t)(a-cutbuf);
		szc=sz-ln_term_sz;
		size_t n=max-1;
		//inter
		for(size_t i=0;i<n;i++){
			char*b=memtrm(a)+ln_term_sz;
			size_t ln=(size_t)(b-a);
			size_t len=ln-ln_term_sz;
			size_t spc_sz=row_pad_sz(len);
			void*v=malloc(spc_sz);
			if(!v)return i;
			memcpy(v,cutbuf+sz,len);
			d[i].data=v;
			d[i].sz=len;
			d[i].spc=spc_sz;
			sz+=ln;
			a=b;
		}
		//last
		l=cutbuf_sz-sz;
		size_t sizen=l+szr;
		size_t spc_sz=row_pad_sz(sizen);
		char*rn=malloc(spc_sz);
		if(!rn)return n;
		memcpy(rn,cutbuf+sz,l);
		memcpy(rn+l,rows[y].data+x,szr);
		d[n].data=rn;
		d[n].sz=sizen;
		d[n].spc=spc_sz;
		//mem
		if(rows_expand(max))return max;
	}
	if(mal_spc_rea(&rows[y],x,szc,sz1r,cutbuf))return max;
	if(one)l=x+szc;
	else rows_insert(d,max,y+1);
	pasted(y-ytext,l,w);
	return 0;
}
static void paste(WINDOW*w){
	row*d=(row*)malloc((cutbuf_r-1)*sizeof(row));
	if(!d)return;
	int r=getcury(w);int c=getcurx(w);
	size_t n=pasting(d,r,c,w);
	for(size_t i=0;i<n;i++){
		free(d[i].data);
	}
	free(d);
}
static void vis(char c,WINDOW*w){
	visual(c);
	wnoutrefresh(w);
	doupdate();
}
static void delete_fast(WINDOW*w,int r,int c,char*data,size_t x,size_t sz){
	int*t=&tabs[tabs_rsz*r];int n=t[0];
	int i=1;
	while(i<=n){
		if(c<=t[i])break;
		i++;
	}
	t[0]=i-1;
	//
	int max=getmaxx(w);
	if(xtext==sz)x_right[r]=false;
	else{
		int k=0;
		while(x<sz){
			char ch=data[x];
			if(ch=='\t'){
				t[i]=c;t[0]=i;i++;
				int j=0;
				while(j<tab_sz){
					mapsel[k+j]=' ';
					j++;
					c++;if(c==max)break;
				}
				k+=j;
			}
			else{mapsel[k]=no_char(ch)?'?':ch;c++;k++;}
			if(c==max)break;
			x++;
		}
		waddnstr(w,mapsel,k);
	}
	if(c<max)wclrtoeol(w);
}
static bool delete_key(size_t y,size_t x,int r,int c,WINDOW*w){
	row*r1=&rows[y];size_t sz=r1->sz;
	if(x==sz){
		size_t yy=y+1;
		if(yy==rows_tot)return false;
		row*r2=&rows[yy];
		if(mal_spc_rea(r1,x,r2->sz,0,r2->data))return true;
		row_del(yy,yy);
		refreshpage(w);
		return false;
	}
	char*data=r1->data;
	for(size_t i=x+1;i<sz;i++){
		data[i-1]=data[i];
	}
	r1->sz--;
	delete_fast(w,r,c,data,x,r1->sz);
	return false;
}
static bool bcsp(size_t y,size_t x,int*rw,int*cl,WINDOW*w){
	int c=cl[0];
	if(xtext==0&&c==0){
		if(y==0)return false;
		row*r0=&rows[y-1];
		row*r1=&rows[y];
		c=end(w,y-1);
		if(mal_spc_rea(r0,r0->sz,r1->sz,0,r1->data))return true;
		if(rw[0]==0)ytext--;
		else rw[0]--;
		cl[0]=c;
		row_del(y,y);
		refreshpage(w);
		return false;
	}
	row*r=&rows[y];
	char*data=r->data;size_t sz=r->sz;
	c-=data[x-1]=='\t'?tab_sz:1;
	for(size_t i=x;i<sz;i++){
		data[i-1]=data[i];
	}
	r->sz--;
	if(c<0){
		xtext--;
		refreshpage(w);
		return false;
	}
	cl[0]=c;
	wmove(w,rw[0],c);//move for addstr
	delete_fast(w,rw[0],c,data,x-1,r->sz);
	return false;
}
static bool enter(size_t y,size_t x,int*r,int*c,WINDOW*w){
	if(rows_expand(1))return true;
	size_t s=rows[y].sz-x;                    size_t spc=row_pad_sz(s);
	void*v=malloc(spc);
	if(!v)return true;
	row rw;
	memcpy(v,rows[y].data+x,s);
	rows[y].sz-=s;
	rw.data=v;rw.sz=s;rw.spc=spc;
	rows_insert(&rw,1,y+1);
	if(r[0]==(getmaxy(w)-1))ytext++;
	else r[0]++;
	xtext=0;c[0]=0;
	refreshpage(w);
	return false;
}
static void type(int cr,WINDOW*w){
	int cl=getcurx(w);
	int rw=getcury(w);
	size_t x=xtext+c_to_xc(cl,rw);
	size_t y=ytext+(size_t)rw;
	size_t xx=x;fixmembuf(&y,&x);
	row*r=&rows[y];
	int rwnr=rw;rw=(int)(y-ytext);
	bool off=rw<rwnr;
	if(off){
		xtext=r->sz;
		cl=0;
	}else{
		cl-=xx-x;
	}
	if(cr==Char_Return){if(enter(y,x,&rw,&cl,w))return;}
	else if(cr==Char_Backspace){if(bcsp(y,x,&rw,&cl,w))return;}
	else if(cr==KEY_DC){if(delete_key(y,x,rw,cl,w))return;}
	else{
		char ch=cr&0xff;
		if(mal_spc_rea(r,x,1,r->sz-x,&ch))return;
		bool is_tab=ch=='\t';
		int s=is_tab?tab_sz:1;
		if(off){
			cl=s;
		}else{
			int colmn=cl;
			cl+=s;
			int max=getmaxx(w);
			if(cl>=max){
				char*d=r->data;
				do{
					cl-=d[xtext]=='\t'?tab_sz:1;
					xtext++;
				}while(cl>=max);
				refreshpage(w);
			}else{
				wmove(w,rw,colmn);
				int n=max-cl;
				winnstr(w,mapsel,n);
				int*t=&tabs[tabs_rsz*rw];
				int a=t[0];
				if(a)if(t[a]+s>=max){t[0]--;a--;}
				int i=1;
				for(;i<=a;i++){
					if(colmn<=t[i])break;
				}
				if(is_tab){
					for(int k=tab_sz;k>0;k--){
						waddch(w,' ');
					}
					t[0]=a+1;
					for(int j=a;i<=j;j--){t[j+1]=t[j]+tab_sz;}
					t[i]=colmn;
				}else{
					waddch(w,no_char(ch)?'?':ch);
					int j=a;
					while(i<=j){
						t[j]=t[j]+1;j--;
					}
				}
				waddstr(w,mapsel);
			}
		}
	}
	if(off)refreshpage(w);
	wmove(w,rw,cl);
}
static bool loopin(WINDOW*w){
	int c;
	for(;;){
		c=wgetch(w);
		int a=movment(c,w);
		if(a>0)return true;
		if(!a){
			const char*s=keyname(c);
			if(!strcmp(s,"^V")){
				vis('V',w);
				int rw=getcury(w);int cl=getcurx(w);
				size_t ybsel=ytext+(size_t)rw;
				size_t xbsel=xtext+c_to_xc(cl,rw);
				size_t yesel=ybsel;size_t xesel=xbsel;
				bool orig=true;
				if(ybsel<rows_tot)if(xbsel<rows[ybsel].sz)if(rows[ybsel].data[xbsel]=='\t'){
					printsel(w,ybsel,xbsel,yesel,xesel);
					wmove(w,rw,cl);
				}
				int z;
				do{
					int b=wgetch(w);
					z=movment(b,w);
					if(z>0)return true;
					else{
						int r=getcury(w);int col=getcurx(w);
						if(!z){
							char v=' ';
							visual_bool=b=='c';
							if(visual_bool){
								if(writemembuf(ybsel,xbsel,yesel,xesel))v='C';
							}else if(b=='d'){
								delete(ybsel,xbsel,yesel,xesel,&r,&col,w);
							}
							visual(v);
							refreshpage(w);
						}else{
							if(testrefresh(z,orig))refreshpage(w);
							size_t y=ytext+(size_t)r;size_t x=xtext+c_to_xc(col,r);
							set1membuf(y,x,&orig,&ybsel,&xbsel,&yesel,&xesel);
							printsel(w,ybsel,xbsel,yesel,xesel);
						}
						wmove(w,r,col);
					}
				}while(z);
			}
			else if(!strcmp(s,"^P"))paste(w);
			else if(!strcmp(s,"^S")){
				int y=getcury(w);int x=getcurx(w);
				if(save(rows,rows_tot,textfile))return true;
				wmove(w,y,x);
			}
			else if(!strcmp(s,"^B")){
				if(textfile){
					if(out_f(textfile,rows,rows_tot)){
						visual_bool=true;
						vis('B',w);
					}
				}
			}
			else if(!strcmp(s,"KEY_F(1)")){
				int cy=getcury(w);int cx=getcurx(w);
				werase(w);
				helpshow(0);
				wnoutrefresh(w);
				wnoutrefresh(stdscr);
				doupdate();
				if(helpin(w)){
					ungetch(c);
					return true;
				}
				wmove(w,cy,cx);
			}
			else if(!strcmp(s,"^Q"))return false;
			else type(c,w);
			//continue;
		}else if(visual_bool){
			visual_bool=false;
			visual(' ');
		}
	}
}
static int normalize(char**c,size_t*size,size_t*r){
	int ok=0;
	char*text_w=c[0];
	size_t sz=size[0];
	char*norm=(char*)malloc(2*sz+1);//-1 ok but,when sz=0,not ok
	if(norm){
		size_t j=0;ok=1;
		for(size_t i=0;i<sz;i++){
			char a=text_w[i];
			if(a=='\n'){
				r[0]++;
				if(ln_term_sz==2){
					norm[j]='\r';j++;ok=-1;
				}
				else if(ln_term[0]=='\r'){a='\r';ok=-1;}
			}else if(a=='\r'){
				r[0]++;
				if(ln_term_sz==2){
					bool rn=((i+1)<sz)&&text_w[i+1]=='\n';
					if(rn){
						norm[j]=a;j++;i++;
						a='\n';}
					else{norm[j]=a;j++;a='\n';ok=-1;}
				}
				else if(ln_term[0]=='\n'){
					bool rn=((i+1)<sz)&&text_w[i+1]=='\n';
					if(rn){i++;a='\n';}
					else{a='\n';ok=-1;}
				}
			}
			norm[j]=a;j++;
		}
		norm[j]=0;size[0]=j;
		free(text_w);c[0]=norm;
	}
	return ok;
}
static void rows_init(size_t size){
	char*b=&text_init_b[size];
	row*z=&rows[0];
	z->data=text_init_b;size_t sz;
	char*a=text_init_b;
	for(size_t i=1;i<rows_tot;i++){
		sz=(size_t)(memtrm(a)-a);
		z->sz=sz;z->spc=0;
		a+=sz+ln_term_sz;
		z=&rows[i];
		z->data=a;
	}
	z->sz=(size_t)(b-a);z->spc=0;
	rows_spc=rows_tot;
}
static int startpage(char*f,size_t*text_sz){
	int ok=0;
	int fd=open(f,O_RDONLY);
	if(fd!=-1){
		size_t size=(size_t)lseek(fd,0,SEEK_END);
		text_init_b=malloc(size);
		if(text_init_b){
			lseek(fd,0,SEEK_SET);
			read(fd,text_init_b,size);
			//
			size_t i=size;
			while(i>0){
				i--;
				if(text_init_b[i]=='\n'){
					if(i&&text_init_b[i-1]=='\r'){
						ln_term[0]='\r';
						ln_term[1]='\n';
						ln_term[2]=0;
						ln_term_sz=2;
					}
					break;
				}else if(text_init_b[i]=='\r'){
					ln_term[0]='\r';
					break;
				}
			}
			text_sz[0]=size;
			ok=normalize(&text_init_b,text_sz,&rows_tot);
		}
		close(fd);
	}
	return ok;
}
static char*getfilebuf(char*cutbuf_file,size_t off){
	int f=open(cutbuf_file,O_RDONLY);
	if(f==-1){
		cutbuf_file[off]='.';
		cutbuf_file+=off;
		f=open(cutbuf_file,O_RDONLY);
	}
	if(f!=-1){
		size_t sz=(size_t)lseek(f,0,SEEK_END);
		if(sz){
			char*v=malloc(sz);
			if(v){
				lseek(f,0,SEEK_SET);
				cutbuf_sz=(size_t)read(f,v,sz);
				if(normalize(&v,&cutbuf_sz,&cutbuf_r)){
					cutbuf=v;cutbuf_spc=cutbuf_sz;
				}else free(v);
			}
		}
		close(f);
	}
	return cutbuf_file;
}
static char*setfilebuf(char*s,char*cutbuf_file){
	size_t sz=strlen(s);size_t i=sz;
	do{
		i--;
		char a=s[i];
		if(a=='/'||a=='\\'){i++;break;}
	}while(i);
	char*h=getenv("HOME");
	if(!h)return cutbuf_file;
	size_t l=strlen(h);
	if(!l)return cutbuf_file;
	sz-=i;
	if(l+sz+7>128)return cutbuf_file;
	sprintf(cutbuf_file,"%s/.%sinfo",h,&s[i]);
	return getfilebuf(cutbuf_file,l-1);
}
static void writefilebuf(char*cutbuf_file){
	if(cutbuf_file[0]){
		int f=open(cutbuf_file,O_WRONLY|O_TRUNC);
		if(f!=-1){
			write(f,cutbuf,cutbuf_sz);
			close(f);
		}
	}
}
static bool color(){
	if(start_color()==ERR)return false;
	if(init_pair(1,COLOR_BLACK,COLOR_WHITE)==ERR)return false;
	if(init_pair(2,COLOR_BLACK,COLOR_CYAN)==ERR)return false;
	return true;
}
int main(int argc,char**argv){
	WINDOW*w1=initscr();
	if(w1!=NULL){
		cbreak();//stty,cooked;relevant for getchar at me
		size_t text_sz;
		int ok=0;
		if(argc==2){
			ok=startpage(argv[1],&text_sz);
			if(ok){
				if(ok<1){
					char txt[]={'N','o','r','m','a','l','i','z','e',' ','l','i','n','e',' ','e','n','d','i','n','g','s',' ','t','o',' ','\\','r',' ',' ','?',' ','n','=','n','o',',',' ','d','e','f','a','u','l','t','=','y','e','s',' ',0};
					//           0   1   2   3   4   5   6   7   8   9  10  11  12  13  14  15  16  17  18  19  20  21  22  23  24  25   26  27  28  29  30  31  32  33  34  35  36  37  38  39  40  41  42  43  44  45  46  47  48  49  50
					if(ln_term_sz==2){txt[28]='\\';txt[29]='n';}
					else if(ln_term[0]=='\n')txt[27]='n';
					puts(txt);
					int c=getchar();
					if(c=='n')ok=0;
				}
				if(ok){
					rows=(row*)malloc(rows_tot*sizeof(row));
					if(rows){
						rows_init(text_sz);
						textfile=argv[1];
					}
					else ok=0;
				}
			}
		}else{
			text_init_b=(char*)malloc(1);
			if(text_init_b){
				rows=(row*)malloc(sizeof(row));
				if(rows){
					text_init_b[0]=0;
					text_sz=0;
					rows[0].data=text_init_b;
					rows[0].sz=0;rows[0].spc=0;
					ok=1;
				}
			}
		}
		if(ok){
			text_init_e=text_init_b+text_sz+1;
			if(color()){
				keypad(w1,true);
				noecho();
				nonl();//no translation,faster
				mousemask(ALL_MOUSE_EVENTS,NULL);
				char cutbuf_file_var[128];
				cutbuf_file_var[0]=0;
				char*cutbuf_file=cutbuf_file_var;
				cutbuf_file=setfilebuf(argv[0],cutbuf_file);
				bool loops=false;
				do{
					int r=getmaxy(w1)-1;
					char*a=realloc(x_right,(size_t)r);
					if(!a)break;
					x_right=a;
					int c=getmaxx(w1);
					tabs_rsz=1+(c/tab_sz);
					if(c%tab_sz)tabs_rsz++;
					void*b=realloc(tabs,sizeof(int)*(size_t)(r*tabs_rsz));
					if(!b)break;
					tabs=(int*)b;
					a=realloc(mapsel,(size_t)c+1);
					if(!a)break;
					mapsel=a;
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
						if(mapsel){
							free(mapsel);
							writefilebuf(cutbuf_file);
						}
					}
				}
				if(cutbuf)free(cutbuf);
			}
		}
		if(text_init_b){
			if(rows){
				text_free(0,rows_tot);
				free(rows);
			}
			free(text_init_b);
		}
		endwin();
	}
	return 0;
}
