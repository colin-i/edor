#include "macro.h"

#ifdef HAVE_CURSES_H
#include<curses.h>
#else
#include"inc/main/curses.h"
#endif
#ifdef HAVE_FCNTL_H
#include<fcntl.h>
#else
#include"inc/main/fcntl.h"
#endif
#ifdef HAVE_STDLIB_H
#include"stdlib.h"
#else
#include"inc/main/stdlib.h"
#endif
#ifdef HAVE_STDIO_H
#include"stdio.h"
#else
#include"inc/main/stdio.h"
#endif
#ifdef HAVE_STRING_H
#include"string.h"
#else
#include"inc/main/string.h"
#endif
#ifdef HAVE_UNISTD_H
#include"unistd.h"
#else
#include"inc/main/unistd.h"
#endif

#include"sep.h"

#ifdef ARM7L
#ifdef HAVE_DLFCN_H
#include<dlfcn.h>
#else
#include"inc/main/armv7/dlfcn.h"
#endif
#ifdef HAVE_LIBUNWIND_H
#include<libunwind.h>
#else
#include"inc/main/armv7/libunwind.h"
#endif
#ifndef HAVE_STDLIB_H
#include"inc/main/armv7/stdlib.h"
#endif
#ifndef HAVE_STDIO_H
#include"inc/main/armv7/stdio.h"
#endif
#ifdef HAVE_SIGNAL_H
#include<signal.h>
#else
#include"inc/main/armv7/signal.h"
#endif
static void AddAddress(unsigned long ip,int address_count) {
	Dl_info info;
	dladdr((void*)ip, &info);
	unsigned long relative_address = ip-(unsigned long)info.dli_fbase;
	char buf[100];
	int n=snprintf(buf,99,"  #%02zu:  0x%lx  %s\r\n", address_count, relative_address, info.dli_sname);
	write(STDOUT_FILENO,&buf,(size_t)n);
}
static void CaptureBacktraceUsingLibUnwind(void*ucontext) {
	// Initialize unw_context and unw_cursor.
	unw_context_t unw_context;// = {};
	unw_getcontext(&unw_context);
	unw_cursor_t  unw_cursor;// = {};
	unw_init_local(&unw_cursor, &unw_context);

	// Get more contexts.
	const mcontext_t* signal_mcontext = &(((const ucontext_t*)ucontext)->uc_mcontext);

	// Set registers.
	unw_set_reg(&unw_cursor, UNW_ARM_R0, signal_mcontext->arm_r0);
	unw_set_reg(&unw_cursor, UNW_ARM_R1, signal_mcontext->arm_r1);
	unw_set_reg(&unw_cursor, UNW_ARM_R2, signal_mcontext->arm_r2);
	unw_set_reg(&unw_cursor, UNW_ARM_R3, signal_mcontext->arm_r3);
	unw_set_reg(&unw_cursor, UNW_ARM_R4, signal_mcontext->arm_r4);
	unw_set_reg(&unw_cursor, UNW_ARM_R5, signal_mcontext->arm_r5);
	unw_set_reg(&unw_cursor, UNW_ARM_R6, signal_mcontext->arm_r6);
	unw_set_reg(&unw_cursor, UNW_ARM_R7, signal_mcontext->arm_r7);
	unw_set_reg(&unw_cursor, UNW_ARM_R8, signal_mcontext->arm_r8);
	unw_set_reg(&unw_cursor, UNW_ARM_R9, signal_mcontext->arm_r9);
	unw_set_reg(&unw_cursor, UNW_ARM_R10, signal_mcontext->arm_r10);
	unw_set_reg(&unw_cursor, UNW_ARM_R11, signal_mcontext->arm_fp);
	unw_set_reg(&unw_cursor, UNW_ARM_R12, signal_mcontext->arm_ip);
	unw_set_reg(&unw_cursor, UNW_ARM_R13, signal_mcontext->arm_sp);
	unw_set_reg(&unw_cursor, UNW_ARM_R14, signal_mcontext->arm_lr);
	unw_set_reg(&unw_cursor, UNW_ARM_R15, signal_mcontext->arm_pc);

	unw_set_reg(&unw_cursor, UNW_REG_IP, signal_mcontext->arm_pc);
	unw_set_reg(&unw_cursor, UNW_REG_SP, signal_mcontext->arm_sp);

	// unw_step() does not return the first IP.
	AddAddress(signal_mcontext->arm_pc,0);
	int address_count=1;
	// Unwind frames one by one, going up the frame stack.
	while (unw_step(&unw_cursor) > 0) {
		unw_word_t ip = 0;
		unw_get_reg(&unw_cursor, UNW_REG_IP, &ip);
		AddAddress(ip,address_count);
		if(address_count==29)break;
		address_count++;
	}
}
static void __attribute__((noreturn)) signalHandler(int sig,struct siginfo *info,void* ucontext){
(void)sig;(void)info;
	CaptureBacktraceUsingLibUnwind(ucontext);
	exit(EXIT_FAILURE);
}
//static void baz(int argc){int *foo = (int*)-1;if(argc==1)sprintf((char*)24,"%d\n", *foo);else free((void*)10);}
#endif

#include"extern.h"

char ln_term[3]="\n";
size_t ln_term_sz=1;
char*textfile=nullptr;
row*rows=nullptr;
size_t rows_tot=1;
size_t ytext=0;
size_t xtext=0;
bool mod_flag=true;

#define Char_Escape 27
static char*mapsel=nullptr;
static char*text_file=nullptr;
static size_t rows_spc=1;
static bool*x_right=nullptr;
static int*tabs=nullptr;
static int tabs_rsz;
static int yhelp;
static bool helpend;
static int phelp;
static char*helptext;
#define hel1 "USAGE\n"
#define hel2 " [filepath]\
\nINPUT\
\nhelp: q(uit),up/down,mouse/touch v.scroll\
\narrows(Shift),home(Ctrl,Alt)/end(Ctrl),page up/down;mouse/touch click or v.scroll\
\nCtrl+v = visual mode; Alt+v = visual line mode\
\n    c = copy\
\n    d = delete\
\n    x = cut\
\n    i = indent (I = flow indent)\
\n    u = unindent (U = flow unindent)\
\nCtrl+p = paste; Alt+p = paste at the beginning of the row\
\ncommand mode: left/right,ctrl+q\
\nCtrl+s = save file\
\nCtrl+o = save file as...\
\nCtrl+g = go to row[,column]\
\nCtrl+f = find text; Alt+f = refind text\
\n    if found\
\n      Enter       = next\
\n      Space       = previous\
\n      Right Arrow = [(next/prev)&] replace\
\n      r           = reset replace text\
\n      R           = modify replace text\
\n    c = cancel\
\n    other key to return\
\nCtrl+u = undo\
\nCtrl+r = redo\
\nAlt +u = undo mode: left=undo,right=redo,other key to return\
\nCtrl+q = quit"//29
static bool visual_bool=false;
static char*cutbuf=nullptr;
static size_t cutbuf_sz=0;
static size_t cutbuf_spc=0;
static size_t cutbuf_r=1;
static char*text_init_b=nullptr;
static char*text_init_e;
static int _rb;static int _cb;
static int _re;static int _ce;

bool no_char(char z){return z<32||z>=127;}
static void tab_grow(WINDOW*w,int r,char*a,size_t sz,int*ptr){
	x_right[r]=xtext<sz;
	if(x_right[r]==false)return;
	int c=0;int cr=0;
	int max=getmaxx(w);
	size_t i=xtext;size_t j=i;
	for(;i<sz&&cr<max;i++){
		cr++;
		char z=a[i];
		if(z=='\t'){
			int n=(int)(i-j);
			waddnstr(w,a+j,n);
			c+=n;
			ptr[ptr[0]+1]=c;ptr[0]++;
			j=i+1;
			cr+=tab_sz-1;
			//wmove(w,r,c);
			int k;if(cr>max)k=max;
			else k=cr;
			for(;c<k;c++){
				waddch(w,' ');
			}
		}else if(no_char(z)/*true*/){
			a[i]='?';char aux=a[i+1];a[i+1]='\0';
			waddstr(w,a+j);a[i+1]=aux;a[i]=z;
			c+=i-j+1;j=i+1;
		}
	}
	if(c<max){
		char aux=a[i];
		a[i]='\0';waddstr(w,a+j);a[i]=aux;
	}
}
void refreshrowsbot(WINDOW*w,int i,int maxy){
	size_t maxx=xtext+(size_t)getmaxx(w);
	do{
		size_t j=ytext+(size_t)i;
		int*ptr=&tabs[tabs_rsz*i];ptr[0]=0;
		wmove(w,i,0);
		if(j<rows_tot){
			size_t sz=rows[j].sz;
			if(sz>maxx)sz=maxx;
			tab_grow(w,i,rows[j].data,sz,ptr);
			if(getcury(w)==i)wclrtoeol(w);
		}else{x_right[i]=false;wclrtoeol(w);}
		i++;
	}while(i<maxy);
}
static void refreshrows(WINDOW*w,int i){
	refreshrowsbot(w,i,getmaxy(w));
}
void refreshpage(WINDOW*w){
	refreshrows(w,0);
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
				if(back/*true*/)wmove(w,r,t);
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
	if(right/*true*/){
		if(ytext+1<rows_tot){
			ytext++;refreshpage(w);
			amove(w,y,x);
		}
	}
	else if(ytext!=0){
		ytext--;refreshpage(w);
		amove(w,y,x);
	}
}
static void printinverted(const char*s){
	attrset(COLOR_PAIR(1));
	addstr(s);
	//attr set here,cause,print"   "
	attrset(0);
}
static void helpposition(){
	int x=getcurx(stdscr);int y=getcury(stdscr);
	move(getmaxy(stdscr)-2,0);
	if(helpend/*true*/)printinverted("BOT");
	else if(yhelp==0)printinverted("TOP");
	else addstr("---");
	move(y,x);
}
static int helpmanag(int n){
	int max=getmaxx(stdscr);
	int i=phelp;
	if(yhelp<n){
		int z=i+max;
		if(i!=0&&helptext[i-1]!='\n')z--;
		do{
			if(helptext[i]=='\n'){i++;break;}
			if(helptext[i]==0)break;
			i++;
		}while(i<z);
		if(helptext[i]=='\n')return i+1;
		return i;
	}
	if(helptext[i-1]=='\n'){
		i--;int j=i;do{
			i--;
		}while(helptext[i]!='\n'&&i!=0);
		if(i!=0)i++;
		int sz=j-i;
		if(sz<=max)return i;
		sz-=max;i+=max;
		max--;int k=-1;
		while(sz>0){sz-=max;k++;}
		return k*max+i;
	}
	i-=max;
	if(i!=0&&helptext[i-1]=='\n')return i;
	return i+1;
}
static void helpshow(int n){
	int max=getmaxx(stdscr);
	int i=phelp;int j=i;
	yhelp=n;int y=0;
	int cstart;
	if(i!=0&&helptext[i-1]!='\n')cstart=1;
	else cstart=0;
	int c=cstart;
	do{
		helpend=helptext[i]==0;
		bool newl=helptext[i]=='\n';
		c++;i++;
		bool is_max=c==max;
		if(newl/*true*/||helpend/*true*/||is_max/*true*/){
			move(y,0);
			int sum=i-j+cstart;
			if(cstart!=0){addch(' ');cstart=0;}
			char aux=helptext[i];helptext[i]='\0';
			addstr(&helptext[j]);
			helptext[i]=aux;
			if(sum<max)clrtoeol();
			y++;
			if(getmaxy(stdscr)-3<y)break;
			j=i;
			if(newl==false){
				if(helptext[i]=='\n'){j++;i=j;}
				else cstart=1;
			}
			c=cstart;
		}
	}while(helpend==false);
	helpposition();
}
static void hmove(int n){
	if(helpend/*true*/&&(n>0))return;
	n+=yhelp;
	if(n<0)return;
	phelp=helpmanag(n);
	helpshow(n);
}
static bool helpin(WINDOW*w){
	int c;
	do{
		c=getch();
		if(c==KEY_MOUSE){
			MEVENT e;
			getmouse(&e);
			if((e.bstate&BUTTON4_PRESSED)!=0)hmove(-1);
			else if((e.bstate&BUTTON5_PRESSED)!=0)hmove(1);
		}else if(c==KEY_DOWN)hmove(1);
		else if(c==KEY_UP)hmove(-1);
		else if(c==KEY_RESIZE)return true;
	}while(c!='q');
	//helpclear();wnoutrefresh(stdscr);
	refreshpage(w);
	return false;
}
static void printhelp(){
	move(getmaxy(stdscr)-1,0);
	printinverted(bar_init());
}
static void sumove(WINDOW*w,int y){tmove(w,y,false);}
static void sdmove(WINDOW*w,int y){tmove(w,y,true);}
static void slmove(WINDOW*w,int x,bool notabs){
	int y=getcury(w);
	if(xtext>0){
		xtext--;
		refreshpage(w);
		if(notabs/*true*/)wmove(w,y,x);
		else{
			amove(w,y,x);
			int newx=getcurx(w);
			if(newx<x&&newx+tab_sz<getmaxx(w))wmove(w,y,newx+tab_sz);
		}
	}
}
static void srmove(WINDOW*w,int x,bool back){
	int y=getcury(w);
	if(x_right[y]/*true*/){
		if(back/*true*/){
			xtext++;
			refreshpage(w);
			amove(w,y,x);
		}else{
			if(rows[ytext+(size_t)y].data[xtext]=='\t')x-=tab_sz-1;
			xtext++;
			refreshpage(w);
			bmove(w,y,x,false);
		}
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
static int home(WINDOW*w,size_t r){
	char*d=rows[r].data;
	size_t sz=rows[r].sz;
	size_t i=0;
	while(i<sz){
		if(d[i]!='\t')break;
		i++;
	}
	if(i<xtext){xtext=i;return 0;}
	else if(i==xtext)return 0;
	int max=getmaxx(w);
	int c=0;while(xtext<i){
		if(c+tab_sz<max){
			i--;c+=tab_sz;
		}else xtext++;
	}
	return c;
}
//1resize,0diff key,-,1refreshed,2no refresh
static int movment(int c,WINDOW*w){
	if(c==KEY_MOUSE){
		MEVENT e;
		getmouse(&e);//==OK is when mousemask is 0, but then nothint at getch
		if((e.bstate&BUTTON4_PRESSED)!=0)tmove(w,getcury(w),false);
		else if((e.bstate&BUTTON5_PRESSED)!=0)tmove(w,getcury(w),true);
		else if((e.bstate&BUTTON1_CLICKED)!=0){amove(w,e.y-1,e.x-1);return -2;}
	}else if(c==KEY_UP){
		int y=getcury(w);
		if(y>0){amove(w,y-1,getcurx(w));return -2;}
		else sumove(w,y);
	}else if(c==KEY_DOWN){
		int y=getcury(w);
		if(y+1<getmaxy(w)){amove(w,y+1,getcurx(w));return -2;}
		else sdmove(w,y);
	}else if(c==KEY_LEFT){
		int x=getcurx(w);
		if(x>0){amove(w,getcury(w),x-1);return -2;}
		else slmove(w,x,true);
	}else if(c==KEY_RIGHT){
		int x=getcurx(w);
		if(x+1<getmaxx(w)){if(bmove(w,getcury(w),x+1,false)/*true*/)return -2;}
		else srmove(w,x,false);
	}else if(c==KEY_HOME){
		xtext=0;int y=getcury(w);
		refreshpage(w);
		wmove(w,y,0);
	}else if(c==KEY_END){
		int y=getcury(w);
		size_t r=ytext+(size_t)y;
		int x;if(r<rows_tot)x=end(w,r);
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
	}else if(c==KEY_SLEFT)slmove(w,getcurx(w),false);
	else if(c==KEY_SRIGHT)srmove(w,getcurx(w),true);
	else if(c==KEY_SF)sdmove(w,getcury(w));
	else if(c==KEY_SR)sumove(w,getcury(w));
	else if(c==KEY_RESIZE){
		return 1;
	}
	else{
		const char*s=keyname(c);
		if(strcmp(s,"kHOM5")==0){
			ytext=0;xtext=0;
			refreshpage(w);
			wmove(w,0,0);
		}else if(strcmp(s,"kEND5")==0){
			size_t max=(size_t)getmaxy(w);
			ytext=rows_tot<=max?0:rows_tot-max;
			size_t a=rows_tot-1;
			int y=(int)(a-ytext);
			int x=end(w,a);
			refreshpage(w);
			//moved by curses, but no add str for line breaks
			wmove(w,y,x);
		}else if(strcmp(s,"kHOM3")==0){
			int y=getcury(w);
			size_t r=ytext+(size_t)y;
			int x;if(r<rows_tot)x=home(w,r);
			else{xtext=0;x=0;}
			refreshpage(w);
			wmove(w,y,x);
		}else return 0;
	}
	return -1;
}
static void fixmembuf(size_t*y,size_t*x){
	if(y[0]>=rows_tot){
		y[0]=rows_tot-1;
		x[0]=rows[y[0]].sz;
		return;
	}
	size_t sz=rows[y[0]].sz;
	if(x[0]>sz)x[0]=sz;
}
size_t sizemembuf(size_t ybsel,size_t xbsel,size_t yesel,size_t xesel){
	if(ybsel==yesel)return xesel-xbsel;
	size_t size=rows[ybsel].sz-xbsel+ln_term_sz;
	for(size_t i=ybsel+1;i<yesel;i++){
		size+=rows[i].sz+ln_term_sz;
	}
	return size+xesel;
}
void cpymembuf(size_t ybsel,size_t xbsel,size_t yesel,size_t xesel,char*buf){
	row*b=&rows[ybsel];
	if(ybsel==yesel){
		memcpy(buf,b->data+xbsel,xesel-xbsel);
		return;
	}
	size_t sz1=b->sz-xbsel;
	memcpy(buf,b->data+xbsel,sz1);
	memcpy(buf+sz1,ln_term,ln_term_sz);
	size_t sz=sz1+ln_term_sz;
	for(size_t i=ybsel+1;i<yesel;i++){
		size_t s=rows[i].sz;
		memcpy(buf+sz,rows[i].data,s);
		sz+=s;
		memcpy(buf+sz,ln_term,ln_term_sz);
		sz+=ln_term_sz;
	}
	memcpy(buf+sz,rows[yesel].data,xesel);
}
static bool writemembuf(size_t ybsel,size_t xbsel,size_t yesel,size_t xesel){
	fixmembuf(&ybsel,&xbsel);
	fixmembuf(&yesel,&xesel);
	if(xesel==rows[yesel].sz){if(yesel<rows_tot-1){yesel++;xesel=0;}}
	else xesel++;
	size_t size=sizemembuf(ybsel,xbsel,yesel,xesel);
	if(cutbuf_spc<size){
		void*v=realloc(cutbuf,size);
		if(v==nullptr)return false;
		cutbuf=(char*)v;cutbuf_spc=size;
	}
	cpymembuf(ybsel,xbsel,yesel,xesel,cutbuf);
	cutbuf_sz=size;cutbuf_r=yesel-ybsel+1;
	return true;
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
static void printrow(WINDOW*w,int r,int b,int e,int c1,int c2){
	int m=mid(r,getmaxx(w));
	if(m<=b){
		printpart(w,c2,e-b);
		return;
	}
	if(e<=m){
		printpart(w,c1,e-b);
		return;
	}
	printpart(w,c1,m-b);
	printpart(w,c2,e-m);
}
static void sel(WINDOW*w,int c1,int c2,int rb,int cb,int re,int ce){
	wmove(w,rb,cb);
	int wd=getmaxx(w);
	if(rb==re){
		printrow(w,rb,cb,ce,c1,c2);
	}else{
		printrow(w,rb,cb,wd,c1,c2);
		for(int r=rb+1;r<re;r++){
			int m=mid(r,wd);
			printpart(w,c1,m);
			printpart(w,c2,wd-m);
		}
		printrow(w,re,0,ce,c1,c2);
	}
}
static size_t v_l_x(size_t y,size_t x,size_t rmax,WINDOW*w){
	if(y<rmax){
		size_t ok=xtext+(size_t)getmaxx(w)-1;
		if(ok<rows[y].sz)return rows[y].sz;
		return ok;
	}else if(y==rmax){
		size_t sz=rows[y].sz;
		if(sz!=0&&x<sz-1)return sz-1;
	}
	return x;
}
static void setmembuf(size_t y,size_t x,bool*orig,size_t*yb,size_t*xb,size_t*ye,size_t*xe,WINDOW*w,bool v_l){
	size_t rmax=rows_tot-1;
	if(orig[0]/*true*/){
		if(y<yb[0]){
			if(v_l/*true*/){
				if(y<=rmax)x=0;
				xb[0]=v_l_x(yb[0],xb[0],rmax,w);
			}else{
				if(y<rmax&&x>rows[y].sz)x=rows[y].sz;
				if(yb[0]<rmax&&xb[0]==rows[yb[0]].sz){
					size_t max=(size_t)getmaxx(w)-1;
					if(rows[yb[0]].sz<xtext+max)xb[0]=xtext+max;
				}
			}
			ye[0]=yb[0];yb[0]=y;
			xe[0]=xb[0];xb[0]=x;
			orig[0]=false;
		}
		else if(y>yb[0]||xb[0]<=x){
			if(v_l/*true*/)x=v_l_x(y,x,rmax,w);
			else if(y<rmax&&x>=rows[y].sz)x=xtext+(size_t)getmaxx(w)-1;
			ye[0]=y;xe[0]=x;
		}
		else{
			if(yb[0]<rmax&&xb[0]==rows[yb[0]].sz){
				size_t max=(size_t)getmaxx(w)-1;
				if(rows[yb[0]].sz<xtext+max)xb[0]=xtext+max;
			}
			ye[0]=yb[0];
			xe[0]=xb[0];xb[0]=x;
			orig[0]=false;
		}
	}else{
		if(ye[0]<y){
			if(v_l/*true*/){
				if(ye[0]<=rmax)xe[0]=0;
				x=v_l_x(y,x,rmax,w);}
			else{
				if(ye[0]<rmax&&xe[0]>rows[ye[0]].sz)xe[0]=rows[ye[0]].sz;
				if(y<rmax&&x>=rows[y].sz)x=xtext+(size_t)getmaxx(w)-1;}
			yb[0]=ye[0];ye[0]=y;
			xb[0]=xe[0];xe[0]=x;
			orig[0]=true;
		}
		else if(ye[0]>y){
			if(v_l/*true*/){if(y<=rmax)x=0;}
			else if(y<rmax&&x>rows[y].sz)x=rows[y].sz;
			if(ye[0]<rmax&&xe[0]>=rows[ye[0]].sz){
				size_t max=(size_t)getmaxx(w)-1;
				if(rows[ye[0]].sz<xtext+max)xe[0]=xtext+max;
			}
			yb[0]=y;xb[0]=x;
		}
		else if(xe[0]<x){
			if(y<rmax&&x>=rows[y].sz)x=xtext+(size_t)getmaxx(w)-1;
			yb[0]=ye[0];
			xb[0]=xe[0];xe[0]=x;
			orig[0]=true;
		}
		else{
			if(v_l/*true*/){if(y<=rmax)x=0;}
			else if(y<rmax&&x>rows[y].sz)x=rows[y].sz;
			yb[0]=y;xb[0]=x;
		}
	}
}
static int xc_to_c(size_t col,int r){
	int*p=&tabs[tabs_rsz*r];
	int n=p[0];
	for(int i=0;i<n;i++){
		p++;
		if((size_t)p[0]<col)col+=tab_sz-1;
		else break;
	}
	return(int)col;
}
size_t c_to_xc(int c,int r){
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
static void unsel(WINDOW*w){
	sel(w,0,0,_rb,_cb,_re,_ce);
}
static void difsel(WINDOW*w,int rb,int cb,int re,int ce){
	bool a;bool b;
	if(_rb<rb||(_rb==rb&&_cb<cb)){
		if(re==_re&&ce==_ce)b=false;else b=true;
		_re=rb;_ce=cb;a=true;
	}else if(re<_re||(re==_re&&ce<_ce)){
		if(rb==_rb&&cb==_cb)b=false;else b=true;
		if(ce==getmaxx(w)){_cb=0;_rb=re+1;}
		else{_rb=re;_cb=ce;}
		a=true;
	}else if(rb<_rb||cb<_cb){
		re=_rb;ce=_cb;b=true;
		a=false;
	}else{// if(_re<re||_ce<ce){
		if(_ce==getmaxx(w)&&_re<re){cb=0;rb=_re+1;}
		else{rb=_re;cb=_ce;}
		b=true;
		a=false;
	}
	if(a/*true*/)unsel(w);
	if(b/*true*/)sel(w,1,2,rb,cb,re,ce);
}
static void printsel(WINDOW*w,size_t ybsel,size_t xbsel,size_t yesel,size_t xesel,int z){
	int wd=getmaxx(w);
	int cright=wd-1;
	int rb;int cb;
	if(ybsel<ytext){
		rb=0;cb=0;}
	else{
		rb=(int)(ybsel-ytext);
		if(xbsel<=xtext)cb=0;
		else if(xbsel<=xtext+c_to_xc(cright,rb))cb=xc_to_c(xbsel-xtext,rb);
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
		else if(xtext<=xesel)ce=xc_to_c(xesel-xtext,re);
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
	ce++;
	if(z!=-2)sel(w,1,2,rb,cb,re,ce);
	else difsel(w,rb,cb,re,ce);
	wattrset(w,0);
	_rb=rb;_cb=cb;
	_re=re;_ce=ce;
}
void visual(char a){
	mvaddch(getmaxy(stdscr)-1,getmaxx(stdscr)-2,a);
	wnoutrefresh(stdscr);
}
static void refreshrowscond(WINDOW*w,size_t y,size_t x,size_t r,size_t n){
	if(y!=ytext||x!=xtext)refreshrows(w,0);
	else refreshrowsbot(w,(int)r,n!=0?getmaxy(w):(int)r+1);
}
static void pasted(size_t r,size_t x,WINDOW*w){
	size_t z1=ytext;size_t z2=xtext;size_t z3=r;
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
	refreshrowscond(w,z1,z2,z3,rws);
	wmove(w,(int)r,c);
}
static bool rows_expand(size_t n){
	size_t rowssize=rows_tot+n;
	if(rowssize>rows_spc){
		row*m=(row*)realloc(rows,rowssize*sizeof(row));
		if(m==nullptr)return true;
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
	sz++;//[i]=0;addstr;=aux
	size_t dif=sz&row_pad;
	if(dif!=0)return sz+((dif^row_pad)+1);
	return sz;
}
bool row_alloc(row*rw,size_t l,size_t c,size_t r){
	size_t sz=l+c+r;
	if(sz>=rw->spc){//[i]=0;addstr;=aux
		char*src=rw->data;char*dst;
		size_t size=row_pad_sz(sz);
		if(text_init_b<=src&&src<text_init_e){
			dst=(char*)malloc(size);
			if(dst==nullptr)return true;
			memcpy(dst,src,l);
			memcpy(dst+l,src+l,r);
		}else{
			dst=(char*)realloc(src,size);
			if(dst==nullptr)return true;
			//src=dst;
		}
		rw->data=dst;
		rw->spc=size;
	}//else dst=src;
	return false;
}
void row_set(row*rw,size_t l,size_t c,size_t r,const char*mid){
	char*d=rw->data;
	size_t j=l+c;size_t i=j+r;size_t k=rw->sz;
	rw->sz=i;
	while(j<i){
		i--;k--;d[i]=d[k];
	}
	d+=l;
	while(c>0){
		d[0]=mid[0];
		d++;mid++;c--;
	}
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
		int n=d[x-1]=='\t'?tab_sz:1;
		if(cl+n>=max)break;
		x--;cl+=n;
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
void mod_set(bool flag){
	mod_flag=flag;
	chtype ch=mod_flag/*true*/?' ':'*';
	mvaddch(getmaxy(stdscr)-1,getmaxx(stdscr)-1,ch);
	wnoutrefresh(stdscr);
}
void deleting(size_t ybsel,size_t xbsel,size_t yesel,size_t xesel){
	row*r1=&rows[ybsel];
	if(ybsel==yesel){
		size_t sz=r1->sz;
		size_t dif=xesel-xbsel;
		char*d=rows[ybsel].data;
		for(size_t i=xesel;i<sz;i++){
			d[i-dif]=d[i];
		}
		r1->sz-=dif;
	}else{
		row_set(r1,xbsel,rows[yesel].sz-xesel,0,rows[yesel].data+xesel);
		row_del(ybsel+1,yesel);
	}
}
bool deleting_init(size_t ybsel,size_t xbsel,size_t yesel,size_t xesel){
	if(ybsel!=yesel){
		size_t se=rows[yesel].sz-xesel;
		size_t sb=rows[ybsel].sz-xbsel;
		if(se>sb){//need deleting part at undo
			return row_alloc(&rows[ybsel],rows[ybsel].sz,se-sb,0);
		}
	}
	return false;
}
static bool delet(size_t ybsel,size_t xbsel,size_t yesel,size_t xesel,int*rw,int*cl,WINDOW*w){
	fixmembuf(&ybsel,&xbsel);
	fixmembuf(&yesel,&xesel);
	if(xesel==rows[yesel].sz){if(yesel<rows_tot-1){yesel++;xesel=0;}}
	else xesel++;
	bool many=ybsel!=yesel;
	if(many/*true*/||xbsel!=xesel){
		if(deleting_init(ybsel,xbsel,yesel,xesel)==false){
			if(undo_add_del(ybsel,xbsel,yesel,xesel)==false){
				deleting(ybsel,xbsel,yesel,xesel);
				size_t z1=ytext;size_t z2=xtext;
				deleted(ybsel,xbsel,rw,cl,w);
				refreshrowscond(w,z1,z2,ybsel-ytext,many);
				if(mod_flag/*true*/)mod_set(false);
				return true;
			}
		}
	}
	return false;
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
static size_t pasting(row*d,size_t y,size_t x,size_t*xe,char*buf,size_t buf_sz,size_t buf_r,bool fromcopy){
	bool one=buf_r==1;
	size_t szc;size_t sz1r;size_t l;
	size_t szr=rows[y].sz-x;
	size_t max=buf_r-1;
	if(one/*true*/){
		szc=buf_sz;sz1r=szr;
		l=x+szc;
	}
	else{
		char*a=memtrm(buf)+ln_term_sz;
		sz1r=0;
		size_t sz=(size_t)(a-buf);
		szc=sz-ln_term_sz;
		size_t n=max-1;
		//inter
		for(size_t i=0;i<n;i++){
			char*b=memtrm(a)+ln_term_sz;
			size_t ln=(size_t)(b-a);
			size_t len=ln-ln_term_sz;
			size_t spc_sz=row_pad_sz(len);
			void*v=malloc(spc_sz);
			if(v==nullptr)return i+1;
			memcpy(v,buf+sz,len);
			d[i].data=(char*)v;
			d[i].sz=len;
			d[i].spc=spc_sz;
			sz+=ln;
			a=b;
		}
		//last
		l=buf_sz-sz;
		size_t sizen=l+szr;
		size_t spc_sz=row_pad_sz(sizen);
		char*rn=(char*)malloc(spc_sz);
		if(rn==nullptr)return max;
		memcpy(rn,buf+sz,l);
		memcpy(rn+l,rows[y].data+x,szr);
		d[n].data=rn;
		d[n].sz=sizen;
		d[n].spc=spc_sz;
		//mem
		if(rows_expand(max)/*true*/)return buf_r;
	}
	if(row_alloc(&rows[y],x,szc,sz1r)==false){
		if(fromcopy/*true*/)if(undo_add(y,x,one/*true*/?y:y+max,l)/*true*/)return buf_r;
		row_set(&rows[y],x,szc,sz1r,buf);
		if(one==false)rows_insert(d,max,y+1);
		xe[0]=l;
		return 0;
	}
	return buf_r;
}
bool paste(size_t y,size_t x,size_t*xe,char*buf,size_t buf_sz,size_t buf_r,bool fromcopy){
	row*d;
	if(buf_r>1){d=(row*)malloc((buf_r-1)*sizeof(row));
		if(d==nullptr)return false;}
	else d=nullptr;
	size_t n=pasting(d,y,x,xe,buf,buf_sz,buf_r,fromcopy);
	if(d!=nullptr){
		for(size_t i=1;i<n;i++){
			free(d[i-1].data);
		}
		free(d);
	}
	return n==0;
}
static void past(WINDOW*w){
	if(cutbuf_sz!=0){
		int r=getcury(w);
		size_t y=ytext+(size_t)r;
		size_t x=xtext+c_to_xc(getcurx(w),r);
		fixmembuf(&y,&x);
		size_t xe;
		if(paste(y,x,&xe,cutbuf,cutbuf_sz,cutbuf_r,true)/*true*/){
			pasted(y-ytext,xe,w);
			if(mod_flag/*true*/)mod_set(false);
			position(getcury(w),getcurx(w));
		}
	}
}
void vis(char c,WINDOW*w){
	visual(c);
	wmove(w,getcury(w),getcurx(w));
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
			else{mapsel[k]=no_char(ch)/*true*/?'?':ch;c++;k++;}
			if(c==max)break;
			x++;
		}
		waddnstr(w,mapsel,k);
	}
	if(c<max)wclrtoeol(w);
}
static void rowfixdel(WINDOW*w,int r,int c,row*rw,size_t i){
	wmove(w,r,c);
	int wd=getmaxx(w);
	char*d=rw->data;
	int*t=&tabs[tabs_rsz*r];
	int a=t[0]+1;
	size_t mx=rw->sz;
	while(c<wd&&i<mx){
		char ch=d[i];
		if(ch!='\t'){
			c++;waddch(w,no_char(ch)/*true*/?'?':ch);
		}else{
			t[a]=c;t[0]++;a++;
			c+=tab_sz;wmove(w,r,c);
		}
		i++;
	}
	x_right[r]=mx!=0;
}
static bool delete_key(size_t y,size_t x,int r,int c,WINDOW*w){
	row*r1=&rows[y];size_t sz=r1->sz;
	if(x==sz){
		size_t yy=y+1;
		if(yy==rows_tot)return false;
		row*r2=&rows[yy];
		if(row_alloc(r1,x,r2->sz,0)==false){
			if(undo_add_del(y,x,yy,0)==false){
				row_set(r1,x,r2->sz,0,r2->data);
				row_del(yy,yy);
				rowfixdel(w,r,c,r1,x);
				if(r+1<getmaxy(w))refreshrows(w,r+1);
				return false;
			}
		}
		return true;
	}
	if(undo_delk(y,x,y,x+1)==false){
		char*data=r1->data;
		for(size_t i=x+1;i<sz;i++){
			data[i-1]=data[i];
		}
		r1->sz--;
		delete_fast(w,r,c,data,x,r1->sz);
		return false;
	}
	return true;
}
static bool bcsp(size_t y,size_t x,int*rw,int*cl,WINDOW*w){
	int c=cl[0];
	if(xtext==0&&c==0){
		if(y==0)return false;
		size_t yy=y-1;
		row*r0=&rows[yy];
		row*r1=&rows[y];
		size_t sz0=r0->sz;
		size_t xx=xtext;c=end(w,yy);
		if(row_alloc(r0,sz0,r1->sz,0)==false){
			if(undo_add_del(yy,sz0,y,0)==false){
				row_set(r0,sz0,r1->sz,0,r1->data);
				row_del(y,y);
				cl[0]=c;
				int r=rw[0];
				if(r==0){
					ytext--;refreshpage(w);
				}
				else{
					if(xtext!=xx)refreshpage(w);
					else{
						rowfixdel(w,r-1,c,r0,sz0);
						refreshrows(w,r);
					}
					rw[0]=r-1;
				}
				return false;
			}
		}
		return true;
	}
	if(undo_bcsp(y,x-1,y,x)==false){
		row*r=&rows[y];
		char*data=r->data;size_t sz=r->sz;
		c-=data[x-1]=='\t'?tab_sz:1;
		for(size_t i=x;i<sz;i++){
			data[i-1]=data[i];
		}
		r->sz--;
		if(xtext!=0){
			int mx=8;
			if(c<mx){
				if(c<0){c=0;xtext--;}
				if(xtext!=0){
					while(c<mx){
						xtext--;
						c+=data[xtext]=='\t'?tab_sz:1;
						if(xtext==0)break;
					}
				}
				refreshpage(w);
				cl[0]=c;
				return false;
			}
		}
		cl[0]=c;
		wmove(w,rw[0],c);//move for addstr
		delete_fast(w,rw[0],c,data,x-1,r->sz);
		return false;
	}
	return true;
}
static bool enter(size_t y,size_t x,int*r,int*c,WINDOW*w){
	if(rows_expand(1)/*true*/)return true;
	char*b=rows[y].data;
	char*d=b;
	char*e=b+x;
	while(d<e&&d[0]=='\t')d++;
	size_t tb=(size_t)(d-b);
	size_t s=rows[y].sz-x;
	size_t sze=tb+s;
	size_t spc=row_pad_sz(sze);
	char*v=(char*)malloc(spc);
	if(v==nullptr)return true;
	if(undo_add(y,x,y+1,tb)==false){
		row rw;
		memset(v,'\t',tb);
		memcpy(v+tb,b+x,s);
		rows[y].sz-=s;
		rw.data=v;rw.sz=sze;rw.spc=spc;
		rows_insert(&rw,1,y+1);
		bool fix=tb>=xtext;
		int row=r[0];
		if(row==(getmaxy(w)-1))ytext++;
		else{
			r[0]++;
			if(fix/*true*/){
				int*t=&tabs[tabs_rsz*row];
				int a=t[0];
				int*p=t+a;
				int*z=p;int cprev=c[0];
				while(p!=t){
					if(p[0]<cprev)break;
					p--;
				}
				t[0]-=z-p;
				c[0]=(int)(tb-xtext)*tab_sz;
				wclrtoeol(w);
				x_right[row]=xtext<rows[y].sz;
				refreshrows(w,row+1);
				return false;
			}
		}
		if(fix/*true*/)c[0]=(int)(tb-xtext)*tab_sz;
		else{xtext=tb;c[0]=0;}
		refreshpage(w);
		return false;
	}
	return true;
}
static void type(int cr,WINDOW*w){
	int cl=getcurx(w);
	int rwnr=getcury(w);
	size_t x=xtext+c_to_xc(cl,rwnr);
	size_t y=ytext+(size_t)rwnr;
	size_t xx=x;fixmembuf(&y,&x);
	row*r=&rows[y];
	int rw=(int)(y-ytext);
	if(rw<rwnr){
		xtext=r->sz;
		cl=0;
		refreshpage(w);
	}else{
		size_t dif=xx-x;
		cl-=dif;
		if(cl<0){
			xtext=r->sz;
			cl=0;
			refreshpage(w);
		}
	}
	if(cr==Char_Return){
		if(enter(y,x,&rw,&cl,w)/*true*/)return;
		position(rw,cl);
	}
	else if(cr==Char_Backspace){
		if(bcsp(y,x,&rw,&cl,w)/*true*/)return;
		position(rw,cl);
	}
	else if(cr==KEY_DC){if(delete_key(y,x,rw,cl,w)/*true*/)return;}
	else{
		char ch=cr&0xff;
		if(row_alloc(r,x,1,r->sz-x)/*true*/)return;
		if(undo_type(y,x,y,x+1)/*true*/)return;
		row_set(r,x,1,r->sz-x,&ch);
		bool is_tab=ch=='\t';
		int s=is_tab/*true*/?tab_sz:1;
		//
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
			if(a!=0)if(t[a]+s>=max){t[0]--;a--;}
			int i=1;
			for(;i<=a;i++){
				if(colmn<=t[i])break;
			}
			if(is_tab/*true*/){
				for(int k=tab_sz;k>0;k--){
					waddch(w,' ');
				}
				t[0]=a+1;
				for(int j=a;i<=j;j--){t[j+1]=t[j]+tab_sz;}
				t[i]=colmn;
			}else{
				waddch(w,no_char(ch)/*true*/?'?':ch);
				int j=a;
				while(i<=j){
					t[j]=t[j]+1;j--;
				}
			}
			waddstr(w,mapsel);
			x_right[rw]=true;
		}
		position(rw,cl);
	}
	wmove(w,rw,cl);
	if(mod_flag/*true*/)mod_set(false);
}
static void indent(bool b,size_t ybsel,size_t*xbsel,size_t yesel,size_t*xesel,WINDOW*w){
	if(ybsel>=rows_tot)return;
	size_t ye;
	if(yesel>=rows_tot)ye=rows_tot;
	else ye=yesel+1;
	if(b/*true*/){
		for(size_t i=ybsel;i<ye;i++){
			row*r=&rows[i];
			if(row_alloc(r,0,1,r->sz)/*true*/)return;
		}
		if(undo_add_ind(ybsel,ye)/*true*/)return;
		for(size_t i=ybsel;i<ye;i++){
			row*r=&rows[i];
			row_set(r,0,1,r->sz,"\t");
		}
		if(mod_flag/*true*/)mod_set(false);
	}else{
		bool something=false;
		for(size_t i=ybsel;i<=ye;i++){
			if(rows[i].sz!=0){something=true;break;}
		}
		if(something/*true*/){
			if(undo_add_ind_del(ybsel,ye)/*true*/)return;
			for(size_t i=ybsel;i<ye;i++){
				row*r=&rows[i];size_t sz=r->sz;
				if(sz!=0){
					char*d=r->data;
					for(size_t j=1;j<sz;j++)d[j-1]=d[j];
					r->sz=sz-1;
				}
			}
			if(mod_flag/*true*/)mod_set(false);
		}
	}
	int rb;if(ybsel<ytext)rb=0;
	else rb=(int)(ybsel-ytext);
	int re=(int)(yesel-ytext)+1;
	int max=getmaxy(w);
	if(re>max)re=max;
	if(b/*true*/){
		if(xbsel!=nullptr){
			xbsel[0]++;xesel[0]++;
			xtext++;
			if(rb!=0)refreshrowsbot(w,0,rb);
			if(re<max)refreshrowsbot(w,re,max);
		}else refreshrowsbot(w,rb,re);
	}else{
		if(xbsel!=nullptr){
			if(xtext!=0){
				xbsel[0]--;xesel[0]--;
				xtext--;
				if(rb!=0)refreshrowsbot(w,0,rb);
				if(re<max)refreshrowsbot(w,re,max);
			}else{
				xbsel[0]=0;xesel[0]=v_l_x(yesel,xesel[0],rows_tot-1,w);
				refreshrowsbot(w,rb,re);
				printsel(w,ybsel,0,yesel,xesel[0],-1);
			}
		}else refreshrowsbot(w,rb,re);
	}
}
static bool visual_mode(WINDOW*w,bool v_l){
	visual('V');
	int rw=getcury(w);int cl=getcurx(w);
	size_t ybsel=ytext+(size_t)rw;
	size_t yesel=ybsel;
	size_t xbsel;size_t xesel;
	bool orig=true;
	size_t rmax=rows_tot-1;
	if(v_l/*true*/){
		if(ybsel<rmax){
			xbsel=0;
			xesel=xtext+(size_t)getmaxx(w)-1;
			if(xesel<rows[yesel].sz)xesel=rows[yesel].sz;
		}
		else{
			xesel=xtext+(size_t)cl;
			if(ybsel==rmax){
				xbsel=0;
				size_t sz=rows[yesel].sz;
				if(sz!=0&&xesel<sz-1)xesel=sz-1;
			}else xbsel=xesel;
		}
	}else{
		xbsel=xtext+c_to_xc(cl,rw);xesel=xbsel;
		if(yesel<rmax&&xesel>=rows[yesel].sz){
			xbsel=rows[ybsel].sz;
			xesel=xtext+(size_t)getmaxx(w)-1;
		}
	}
	printsel(w,ybsel,xbsel,yesel,xesel,-1);
	wmove(w,rw,cl);
	int z;
	do{
		int b=wgetch(w);
		z=movment(b,w);
		if(z==1)return true;
		else{
			int r=getcury(w);int col=getcurx(w);
			if(z==0){
				if(b=='I'){z=-1;indent(true,ybsel,&xbsel,yesel,&xesel,w);}
				else if(b=='U'){
					z=-1;
					bool edge=xtext==0;
					indent(false,ybsel,&xbsel,yesel,&xesel,w);
					if(edge/*true*/){amove(w,r,col);continue;}
				}
				else{
					char v=' ';
					visual_bool=b=='c';
					if(visual_bool/*true*/){
						if(writemembuf(ybsel,xbsel,yesel,xesel)/*true*/){v='C';unsel(w);}
					}else if(b=='d'){
						if(delet(ybsel,xbsel,yesel,xesel,&r,&col,w)/*true*/)
							if(orig/*true*/)position(r,col);
					}else if(b=='x'){
						if(writemembuf(ybsel,xbsel,yesel,xesel)/*true*/){
							if(delet(ybsel,xbsel,yesel,xesel,&r,&col,w)/*true*/)
								if(orig/*true*/)position(r,col);
						}
					}else{
						if(b=='i'){
							indent(true,ybsel,nullptr,yesel,nullptr,w);
							amove(w,r,col);
							col=getcurx(w);
						}else if(b=='u'){
							indent(false,ybsel,nullptr,yesel,nullptr,w);
							amove(w,r,col);
							col=getcurx(w);
						}
						unsel(w);
					}
					visual(v);
				}
			}else{
				position(getcury(w),getcurx(w));
				size_t y=ytext+(size_t)r;size_t x=xtext+c_to_xc(col,r);
				setmembuf(y,x,&orig,&ybsel,&xbsel,&yesel,&xesel,w,v_l);
				printsel(w,ybsel,xbsel,yesel,xesel,z);
			}
			wmove(w,r,col);
		}
	}while(z!=0);
	return false;
}
static bool find_mode(int nr,WINDOW*w){
	char*args[2];
	args[0]=(char*)nr;
	args[1]=(char*)w;
	int r=command((char*)args);
	if(r==-2)return true;
	else if(r!=0){
		wmove(w,getcury(w),getcurx(w));
	}
	return false;
}
static bool loopin(WINDOW*w){
	int c;
	for(;;){
		c=wgetch(w);
		int a=movment(c,w);
		if(a==1)return true;
		if(a!=0){
			if(visual_bool/*true*/){
				visual_bool=false;
				visual(' ');
			}else if(bar_clear()/*true*/)wnoutrefresh(stdscr);
			position(getcury(w),getcurx(w));
		}else if(c==Char_Escape){
			nodelay(w,true);
			int z=wgetch(w);
			nodelay(w,false);
			if(z=='v'){if(visual_mode(w,true)/*true*/)return true;}
			else if(z=='p'){wmove(w,getcury(w),0);past(w);}
			else if(z=='f'){if(find_mode(3,w)/*true*/)return true;}
			else if(z=='u'){vis('U',w);undo_loop(w);vis(' ',w);}
		}else{
			const char*s=keyname(c);
			if(strcmp(s,"^V")==0){
				if(visual_mode(w,false)/*true*/)return true;
			}
			else if(strcmp(s,"^P")==0)past(w);
			else if((strcmp(s,"^S")==0)||strcmp(s,"^O")==0){
				char*d=textfile;
				int ret;
				if(s[1]=='S'){
					ret=save();
				}else{char aa=0;ret=command(&aa);}
				if(ret!=0){
					if(ret==1){
						if(d!=textfile)text_file=textfile;
						mod_set(true);
						undo_save();
					}
					else if(ret==-2)return true;
				}else err_set(w);
				wmove(w,getcury(w),getcurx(w));
			}
			else if(strcmp(s,"^G")==0){
				char aa=1;
				int r=command(&aa);
				if(r==1){
					centering(w,nullptr,nullptr);
				}
				else if(r>-2)wmove(w,getcury(w),getcurx(w));
				else return true;
			}
			else if(strcmp(s,"^F")==0){
				if(find_mode(2,w)/*true*/)return true;
			}else if(strcmp(s,"^U")==0){
				undo(w);
			}else if(strcmp(s,"^R")==0){
				redo(w);
			}else if(strcmp(s,"KEY_F(1)")==0){
				int cy=getcury(w);int cx=getcurx(w);
				phelp=0;
				helpshow(0);
				int mx=getmaxy(stdscr)-2;
				for(int i=getcury(stdscr)+1;i<mx;i++){move(i,0);clrtoeol();}
				if(helpin(w)/*true*/){
					ungetch(c);
					return true;
				}
				wmove(w,cy,cx);
			}
			else if(strcmp(s,"^Q")==0){
				if(mod_flag==false){
					bar_clear();//errors
					int q=question("And save");
					if(q==1){
						q=save();
						if(q==0)err_set(w);
					}
					if(q==-2)return true;
					else if(q==0){
						wnoutrefresh(stdscr);
						wmove(w,getcury(w),getcurx(w));
						continue;
					}
				}
				return false;
			}
			else type(c,w);
			//continue;
		}
	}
}
static int normalize(char**c,size_t*size,size_t*r){
	int ok=0;
	char*text_w=c[0];
	size_t sz=size[0];
	char*norm=(char*)malloc(2*sz+1);//-1 ok but,when sz=0,not ok
	if(norm!=nullptr){
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
					if(((i+1)<sz)&&text_w[i+1]=='\n'){
						norm[j]=a;j++;i++;
						a='\n';}
					else{norm[j]=a;j++;a='\n';ok=-1;}
				}
				else if(ln_term[0]=='\n'){
					if(((i+1)<sz)&&text_w[i+1]=='\n'){i++;a='\n';}
					else{a='\n';ok=-1;}
				}
			}
			norm[j]=a;j++;
		}
		norm[j]='\0';size[0]=j;
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
static int startfile(char*f,size_t*text_sz){
	int ok=0;
	int fd=open(f,O_RDONLY);
	if(fd!=-1){
		if(is_dir(fd)/*true*/){
			putchar('\"');
			size_t n=strlen(f);
			for(size_t i=0;i<n;i++){
				putchar(f[i]);
			}
			//puts(f);
			puts("\" is a directory");
		}
		else{
			size_t size=(size_t)lseek(fd,0,SEEK_END);
			text_init_b=(char*)malloc(size);
			if(text_init_b!=nullptr){
				lseek(fd,0,SEEK_SET);
				read(fd,text_init_b,size);
				//
				size_t i=size;
				while(i>0){
					i--;
					if(text_init_b[i]=='\n'){
						if(i!=0&&text_init_b[i-1]=='\r'){
							ln_term[0]='\r';
							ln_term[1]='\n';
							ln_term[2]='\0';
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
		}
		close(fd);
	}
	return ok;
}
static void getfilebuf(char*cutbuf_file){//,size_t off){
	int f=open(cutbuf_file,O_RDONLY);
	/*if(f==-1){
		char store=cutbuf_file[off];
		cutbuf_file[off]='.';
		f=open(cutbuf_file+off,O_RDONLY);
		cutbuf_file[off]=store;
	}*/
	if(f!=-1){
		size_t sz=(size_t)lseek(f,0,SEEK_END);
		if(sz!=0){
			char*v=(char*)malloc(sz);
			if(v!=nullptr){
				lseek(f,0,SEEK_SET);
				cutbuf_sz=(size_t)read(f,v,sz);
				if(normalize(&v,&cutbuf_sz,&cutbuf_r)!=0){
					cutbuf=v;cutbuf_spc=cutbuf_sz;
				}else free(v);
			}
		}
		close(f);
	}
}
static bool help_init(char*f,size_t szf){
	size_t sz1=sizeof(hel1)-1;
	size_t sz2=sizeof(hel2);
	char*a=(char*)malloc(sz1+szf+sz2);
	if(a!=nullptr){
		helptext=a;
		memcpy(a,hel1,sz1);
		a+=sz1;memcpy(a,f,szf);
		memcpy(a+szf,hel2,sz2);
		return true;
	}
	return false;
}
static bool setfilebuf(char*s,char*cutbuf_file){
#if ((!defined(USE_FS)) && (!defined(USE__FS)))
	set_path_separator(s);
#endif
	size_t sz=strlen(s);size_t i=sz;
	do{
		i--;
		char a=s[i];
		if(a==path_separator){i++;break;}
	}while(i!=0);
	bool b=help_init(&s[i],sz-i);
	char*h=getenv("HOME");
	if(h!=nullptr){
		size_t l=strlen(h);
		if(l!=0){
			if(l+(sz-i)+7<=128){
				sprintf(cutbuf_file,"%s%c.%sinfo",h,path_separator,&s[i]);
				getfilebuf(cutbuf_file);//l-1
			}
		}
	}
	return b;
}
static void writefilebuf(char*cutbuf_file){
	if(cutbuf_file[0]!=0){
		int f=open(cutbuf_file,O_WRONLY|O_TRUNC);
		if(f!=-1){
			write(f,cutbuf,cutbuf_sz);
			close(f);
		}
	}
}
static void color(){
	if(start_color()!=ERR){
		if(init_pair(1,COLOR_BLACK,COLOR_WHITE)!=ERR){//TERM vt100
			init_pair(2,COLOR_BLACK,COLOR_CYAN);
		}
	}
}

static void proced(char*comline){
	char cutbuf_file[128];
	cutbuf_file[0]='\0';
	if(setfilebuf(comline,cutbuf_file)/*true*/){
		bool loops=false;
		int cy=0;int cx=0;
		int r=getmaxy(stdscr)-1;int old_r=r;
		do{
			void*a=realloc(x_right,(size_t)r);
			if(a==nullptr)break;
			x_right=(bool*)a;
			int c=getmaxx(stdscr);
			tabs_rsz=1+(c/tab_sz);
			if((c%tab_sz)!=0)tabs_rsz++;
			void*b=realloc(tabs,sizeof(int)*(size_t)(r*tabs_rsz));
			if(b==nullptr)break;
			tabs=(int*)b;
			a=realloc(mapsel,(size_t)c+1);
			if(a==nullptr)break;
			mapsel=(char*)a;
			WINDOW*w=newwin(r,c,0,0);
			if(w!=nullptr){
				keypad(w,true);
				refreshpage(w);
				wmove(w,cy,cx);
				printhelp();
				if(r<old_r)clrtoeol();//resize to up,is over text
				old_r=r;
				if(mod_flag==false)mod_set(false);
				else wnoutrefresh(stdscr);
				position_reset();
				position(cy,cx);
				loops=loopin(w);
				if(loops/*true*/){//is already resized and the cursor fits in the screen, not in the new size
					cy=getcury(w);
					r=getmaxy(stdscr)-1;
					if(cy==r){
						cy=r-1;
						if(ytext+1<rows_tot)ytext++;
					}
					cx=getcurx(w);
					//c=getmaxx(w1);never if(cx>=c)
				}
				delwin(w);
			}else break;
		}while(loops/*true*/);
		if(x_right!=nullptr){
			free(x_right);
			if(tabs!=nullptr){
				free(tabs);
				if(mapsel!=nullptr){
					free(mapsel);
					writefilebuf(cutbuf_file);
					undo_free();
				}
			}
		}
		free(helptext);
	}
	if(cutbuf!=nullptr)free(cutbuf);
}
int main(int argc,char**argv){
	#ifdef ARM7L
	struct sigaction signalhandlerDescriptor;
	memset(&signalhandlerDescriptor, 0, sizeof(signalhandlerDescriptor));
	signalhandlerDescriptor.sa_flags = SA_SIGINFO;//SA_RESTART | SA_ONSTACK;
	signalhandlerDescriptor.sa_sigaction = signalHandler;
	sigaction(SIGSEGV, &signalhandlerDescriptor, nullptr);
	//baz(argc);
	#endif
	WINDOW*w1=initscr();
	if(w1!=nullptr){
		raw();//stty,cooked;relevant for getchar at me
		size_t text_sz;
		int ok=0;
		if(argc!=2||new_visual(argv[1])/*true*/){
			text_init_b=(char*)malloc(1);
			if(text_init_b!=nullptr){
				rows=(row*)malloc(sizeof(row));
				if(rows!=nullptr){
					text_init_b[0]='\0';
					text_sz=0;
					rows[0].data=text_init_b;
					rows[0].sz=0;rows[0].spc=0;
					ok=1;
					text_init_e=text_init_b+1;
				}
			}
		}else{
			ok=startfile(argv[1],&text_sz);
			if(ok!=0){
				if(ok<1){
					char txt[]={'N','o','r','m','a','l','i','z','e',' ','l','i','n','e',' ','e','n','d','i','n','g','s',' ','t','o',' ','\\','r',' ',' ','?',' ','n','=','n','o',',',' ','d','e','f','a','u','l','t','=','y','e','s','\r','\0'};
					//           0   1   2   3   4   5   6   7   8   9   10  11  12  13  14  15  16  17  18  19  20  21  22  23  24  25  26   27  28  29  30  31  32  33  34  35  36  37  38  39  40  41  42  43  44  45  46  47  48  49   50
					if(ln_term_sz==2){txt[28]='\\';txt[29]='n';}
					else if(ln_term[0]=='\n')txt[27]='n';
					puts(txt);
					int c=getchar();
					if(c=='n')ok=0;
				}
				if(ok!=0){
					rows=(row*)malloc(rows_tot*sizeof(row));
					if(rows!=nullptr){
						rows_init(text_sz);
						textfile=argv[1];
						text_init_e=text_init_b+text_sz+1;
					}
					else ok=0;
				}
			}
		}
		if(ok!=0){
			color();
			WINDOW*pw=position_init();
			if(pw!=nullptr){
				keypad(w1,true);
				noecho();
				nonl();//no translation,faster
				mousemask(ALL_MOUSE_EVENTS,nullptr);//for error, export TERM=vt100
				proced(argv[0]);
				delwin(pw);
			}
		}
		if(text_init_b!=nullptr){
			if(rows!=nullptr){
				text_free(0,rows_tot);
				free(rows);
				//puts(text_file
			}
			free(text_init_b);
		}
		endwin();
		if(text_file!=nullptr)puts(text_file);
	}
	return 0;
}
