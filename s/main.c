
#define is_main_c
#ifdef HAVE_STDDEF_H
#include<stddef.h>
#else
#include"inc/stddef.h"
#endif

#include "top.h"

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
#ifdef HAVE_STDIO_H
#include<stdio.h>
#else
#include"inc/main/stdio.h"
#endif
#ifdef HAVE_STDLIB_H
#include<stdlib.h>
#else
#include"inc/main/stdlib.h"
#endif
#ifdef HAVE_STRING_H
#include<string.h>
#else
#include"inc/main/string.h"
#endif
#ifdef HAVE_UNISTD_H
#include<unistd.h>
#else
#include"inc/main/unistd.h"
#endif
#ifdef HAVE_TIME_H
#include<time.h>
#else
#include"inc/main/time.h"
#endif
#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#else
#include"inc/main/sys_stat.h"
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
// This method can only be used on 32-bit ARM
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
static void __attribute__((noreturn)) signalHandler(int sig,siginfo_t *info,void* ucontext){
(void)sig;(void)info;
	CaptureBacktraceUsingLibUnwind(ucontext);
	exit(EXIT_FAILURE);
}
//static void baz(int argc){int *foo = (int*)-1;if(argc==1)sprintf((char*)24,"%d\n", *foo);else free((void*)10);}
#endif

#include"base.h"
#include "main.h"

#define no_clue (size_t)-1

#define normalize_yes -1
#define normalize_err 0
#define normalize_ok 1
#define normalize_char signed char

char ln_term[3]="\n";
size_t ln_term_sz=1;
char*textfile=nullptr;
row*rows=nullptr;
size_t rows_tot=1;
size_t ytext=0;
row_dword xtext=0;//and what about when xtext + xc goes size_t ? is ok, nothing is lost, tested, (size_t)xtext cast is same without cast, but is recommended
bool mod_flag=true;
bool ocompiler_flag=false;
size_t aftercall;
size_t clue=no_clue;size_t normalize_clue=no_clue;

static char*mapsel=nullptr;
//static char*text_file=nullptr;
static size_t rows_spc=1;//at rows_expand
static bool*x_right=nullptr;
static int*tabs=nullptr;
static int tabs_rsz;
static int yhelp;
static bool helpend;
static int phelp;
static char*helptext;
static time_t hardtime=0;
static char*restorefile=nullptr;
static char restorefile_buf[max_path_0];
static char restorefile_buf2[max_path_0];
static char*editingfile=nullptr;
static char editingfile_buf[max_path_0];
static char editingfile_buf2[max_path_0];

static mmask_t stored_mouse_mask=0;
#define stored_mouse_mask_q stored_mouse_mask!=0
static bool indent_flag=false;
#define mask_size 1
//#define mask_nomask 0
#define mask_mouse 1
#define mask_indent 2
#define mask_insensitive 4
#define mask_ocompiler 8
#define mask_splits 0x10
#define mask_filewhites 0x20
static char prefs_file[max_path_0]={'\0'};//only the first byte is set
static char*ocode_extension_new=nullptr;

//static bool visual_bool=false;
static char*cutbuf=nullptr;
static size_t cutbuf_sz=0;
static size_t cutbuf_spc=0;
static size_t cutbuf_r=1;
static char*text_init_b=nullptr;
static char*text_init_e;//is init? malloc to new : realloc. and to free or not to free.
static int _rb;static int _cb;
static int _re;static int _ce;
static int topspace=1;
//left,syntax,right individual size
#define contentmarginsize 1
static int leftspace=contentmarginsize+contentmarginsize;
#define view_margin 8
static WINDOW*leftcontent=nullptr;
static WINDOW*rightcontent=nullptr;
static char at_right_mark='>';
static char at_left_mark='<';
static char at_content_nomark=' ';
static WINDOW*syntaxcontent=nullptr;
bool filewhites_flag=false;
char*filewhites_extension=(char*)"yml";
static char*filewhites_extension_new=nullptr;
#define tab_protocol char
tab_protocol tab_sz=6;
static int user_return=EXIT_SUCCESS;

#define splits_activated 'S'
#define splits_activated_mixless 'h'
#define splits_deactivated '_'

bool no_char(char z){return z<32||z>=127;}
static size_t tab_grow(WINDOW*w,char*a,size_t sz,int*ptr){
	int c=0;int cr=0;
	int max=getmaxx(w);
	size_t i=(size_t)xtext;size_t j=i;
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
	if(c<max){//we are here because there is space to maxx (if was ended in a tab or no_char, it is not this case)
		if(i!=j){//[j,i) is normal text
			char aux=a[i];
			a[i]='\0';waddstr(w,a+j);a[i]=aux;
			c+=i-j;//to compare if still there is space to maxx
		}
		if(c<max)wclrtoeol(w);//blank space until maxx
	}
	return i;
}
static void printsyntax(int pair,int y){
	wattrset(syntaxcontent,COLOR_PAIR(pair));
	mvwaddch(syntaxcontent,y,0,' ');
}
void refreshrowsbot(WINDOW*w,int i,int maxy){
	size_t maxx=(size_t)xtext+getmaxx(w);
	do{
		size_t j=ytext+(size_t)i;
		int*ptr=&tabs[tabs_rsz*i];ptr[0]=0;
		wmove(w,i,0);
		char at_left=at_content_nomark;char at_right=at_content_nomark;
		if(j<rows_tot){
			row_dword sz=rows[j].sz;
			if(xtext>0)if(sz>0)at_left=at_left_mark;//there is text at left

			size_t maxsz=sz>maxx?maxx:sz;
			x_right[i]=xtext<maxsz;
			if(x_right[i]==false)//the text is left of xtext, or is not
				wclrtoeol(w);//clear here is all, another clear is in tab_grow
			else{
				size_t x=tab_grow(w,rows[j].data,maxsz,ptr);
				if(x<sz)at_right=at_right_mark;//there is text at right
			}

			//if(getcury(w)==i)wclrtoeol(w);
			//this was the case when there was nothing on the row or xtext was big and nothing to print
			//was moved inside tab_grow

			if(ocompiler_flag/*true*/){
				if(aftercall<=j)printsyntax(color_d,i);
				else printsyntax(color_0,i);//when coming back
			}else printsyntax(color_0,i);//when coming back
		}else{
			x_right[i]=false;
			wclrtoeol(w);
			printsyntax(color_c,i);
		}
		mvwaddch(leftcontent,i,0,at_left);
		mvwaddch(rightcontent,i,0,at_right);
		i++;
	}while(i<maxy);
	wnoutrefresh(leftcontent);
	wnoutrefresh(syntaxcontent);
	wnoutrefresh(rightcontent);
}
static void content_at_right(int i){
	mvwaddch(rightcontent,i,0,at_right_mark);
	wnoutrefresh(rightcontent);
}
static void no_content_at_right(int i){
	mvwaddch(rightcontent,i,0,at_content_nomark);
	wnoutrefresh(rightcontent);
}
#define no_content_at_right_if(i) if((char)mvwinch(rightcontent,i,0)==at_right_mark)no_content_at_right(i);

static void bmove(WINDOW*w,int r,int c,bool back){
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
				//		return false;
					}
				}
				//return true;
			}
		}
	}
	//return true;
}
static void amove(WINDOW*w,int r,int c){
	bmove(w,r,c,true);
}
static void vumove(WINDOW*w,int len){
	int y=getcury(w);
	if(y>len-1)amove(w,y-len,getcurx(w));
	else if(ytext!=0){
		int x=getcurx(w);
		ytext=(size_t)len>ytext?0:ytext-(size_t)len;
		refreshpage(w);
		amove(w,y,x);
	}
}
static void vdmove(WINDOW*w,int len){
	int y=getcury(w);
	if(y+len<getmaxy(w))amove(w,y+len,getcurx(w));
	else if(ytext+1<rows_tot){
		int x=getcurx(w);
		ytext=ytext+(size_t)len>=rows_tot?rows_tot-1:ytext+(size_t)len;
		refreshpage(w);
		amove(w,y,x);
	}
}
static void vuNmove(WINDOW*w,int y,size_t n){
	if(ytext!=0){
		if(ytext<n)ytext=0;
		else ytext-=n;
		int x=getcurx(w);
		refreshpage(w);
		amove(w,y,x);
	}
}
#define vu1move(w,y) vuNmove(w,y,1)
static void vdNmove(WINDOW*w,int y,size_t n){
	if(rows_tot-1!=ytext){
		if(ytext+n>=rows_tot)ytext=rows_tot-1;
		else ytext+=n;
		int x=getcurx(w);
		refreshpage(w);
		amove(w,y,x);
	}
}
#define vd1move(w,y) vdNmove(w,y,1)
void printinverted(const char*s){
	attrset(COLOR_PAIR(color_a));
	addstr(s);
	//attr set here,cause,print"   "
	attrset(color_0);
}
static void helpposition(){
	move(getmaxy(stdscr)-2,0);
	if(helpend/*true*/)printinverted("BOT");
	else if(yhelp==0)printinverted("TOP");
	else addstr("---");
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
static int helpshow(int n){
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
			//int sum=i-j+cstart;
			if(cstart!=0){
				addch(' ');//splitted rows still one space at right
				cstart=0;
			}
			char aux=helptext[i];helptext[i]='\0';
			addstr(&helptext[j]);
			helptext[i]=aux;
			//old text seems to go anyway without this
			//if(sum<max)clrtoeol();
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
	int x=getcurx(stdscr);
	if(x!=0)for(;x!=max;x++)addch(' ');//seems that prelast row need this, a color switch will be at next last row
	return y;
}

#define visual_write_at(a,b) mvaddch(getmaxy(stdscr)-1,getmaxx(stdscr)-b,a)
#define visual_write(a) visual_write_at(a,2)
void visual(char a){
	visual_write(a);
	wnoutrefresh(stdscr);
}

char orig_key(char now_key,char mod){
	key_struct*now=&keys[now_key-'A'];
	unsigned char ix=now->index;
	char orig=keys_row_orig[ix]+'A';
	return orig+mod;
}
#define orig_lowkey(a) orig_key(a,A_to_a)
void orig_key_show(show_key_struct*s){
	visual(orig_key(s->key,s->add));//it looks like only mvaddch is enough but at a command_rewrite(example Alt+g) will erase the cursor window if not calling wnoutrefresh now
}

static void helpshowlastrow(int rw){
	int i=3;//with respect to helpposition
	move(rw,i);
	char stats=5+1+2;
	int max=getmaxx(stdscr)-stats;
	while(i<max){
		addch(' ');
		i++;
	}
	addch(stored_mouse_mask_q?orig_upkey(key_mouse):orig_lowkey(key_mouse));//maybe on touchscreen tablet is dominant
	addch(indent_flag/*true*/?orig_upkey(key_indents):orig_lowkey(key_indents));
	addch(insensitive/*true*/?orig_upkey(key_insens):orig_lowkey(key_insens));
	addch(ocompiler_flag/*true*/?orig_upkey(key_ocomp):orig_lowkey(key_ocomp));//i'm using otoc with gdb for new code
	addch(splits_flag/*true*/?orig_upkey(key_actswf):orig_lowkey(key_actswf));
	addch(filewhites_flag/*true*/?orig_upkey(key_whites):orig_lowkey(key_whites));
	addch(' ');
	addch(split_reminder_c>=split_yes_mixless?(split_reminder_c==split_yes_mix?splits_activated:splits_activated_mixless):splits_deactivated);
	//1@: else at my compilers, and also gcc, is faster: if is with new asm jump instruction. but still not counting on that and aspire to fast first
	//is same asm speed with/without false, still why at two of these first is the beta?
	//is beta at me, someone else can use this code. at me 1@ is ok for them.
	helpposition();
}
static void hmove(int n){
	if(helpend/*true*/&&(n>0))return;
	n+=yhelp;
	if(n<0)return;
	phelp=helpmanag(n);
	helpshow(n);
	helpposition();
}
static void topspace_clear(){
	//first write is not here
	move(0,0);//is not here
	clrtoeol();//if name is shorter will let text
}
#define write_the_title(a) printinverted(a)
static void write_title(){
	write_the_title(textfile);
}
//Button 2 is the middle one
static bool helpin(WINDOW*w){
	int c;
	do{
		c=getch();
		if(c==KEY_MOUSE){
			MEVENT e;//switch case? a label can only be part of a statement and a declaration is not a statement
#ifndef OTHER_CURSES
			getmouse(&e);
#else
			nc_getmouse(&e);
#endif
			if((e.bstate&BUTTON4_PRESSED)!=0)hmove(-1);
			else
		#ifdef BUTTON5_PRESSED
			if((e.bstate&BUTTON5_PRESSED)!=0)
		#else
			if(e.bstate==0)     // at wheel down (libncurses5 at bionic)
		#endif
			hmove(1);
		}else if(c==KEY_DOWN){hmove(1);}
		else if(c==KEY_UP){hmove(-1);}
		else if(c==KEY_RESIZE){return true;}
	}while(c!='q');
	//helpclear();wnoutrefresh(stdscr);

	//need to clear first line anyway
	topspace_clear();
	if(textfile!=nullptr)write_title();
	wnoutrefresh(stdscr);//doupdate is not enough

	refreshpage(w);
	return false;
}
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
static int endmv(WINDOW*w,size_t r,bool minusone){
	size_t sz=rows[r].sz;
	if(minusone/*true*/&&sz>0)sz--;
	if(xtext>=sz){xtext=sz;return 0;}//here xtext is not adjusted, is normal
	char*b=rows[r].data;
	char*s=b+sz;
	int n=getmaxx(w)-1;int m=0;
	do{
		s--;m+=s[0]=='\t'?tab_sz:1;
		if((s-b)==xtext)break;
	}while(m<n);
	if(m>n){
		s++;m-=tab_sz;
	}
	xtext=(s-b);
	return m;
}
#define end(w,r) endmv(w,r,false)
static void endmov(WINDOW*w,bool minusone){
	int y=getcury(w);
	size_t r=ytext+(size_t)y;
	row_dword xcare=xtext;
	int x;if(r<rows_tot)x=endmv(w,r,minusone);
	else{xtext=0;x=0;}
	if(xtext!=xcare)refreshpage(w);
	wmove(w,y,x);
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

int xc_to_c(size_t col,int r){
	int*p=&tabs[tabs_rsz*r];
	int n=p[0];
	for(int i=0;i<n;i++){
		p++;
		if((size_t)p[0]<col)col+=tab_sz-1;
		else break;
	}
	return(int)col;
}
int c_to_xc(int c,int r){
	int*p=&tabs[tabs_rsz*r];
	int n=p[0];int x=c;
	for(int i=0;i<n;i++){
		p++;
		//cright can be in tab
		if((p[0]+(tab_sz-1))<c)x-=tab_sz-1;
		else break;
	}
	return x;
}

static void fixmembuf(size_t*y,size_t*x){
	if(y[0]>=rows_tot){
		y[0]=rows_tot-1;
		x[0]=rows[y[0]].sz;
		return;
	}
	row_dword sz=rows[y[0]].sz;
	if(x[0]>sz)x[0]=sz;
}
static bool fixed_y(size_t*y,row_dword*x,int r){
	*y=ytext+(size_t)r;
	if(*y>=rows_tot){
		*y=rows_tot-1;
		*x=rows[*y].sz;
		return false;
	}
	return true;
}
void fixed_x(size_t y,row_dword*x,int r,int c){
	size_t xc=(size_t)xtext+c_to_xc(c,r);
	row_dword sz=rows[y].sz;
	if(xc>sz)*x=sz;
	else *x=xc;
}
void fixed_yx(size_t*y,row_dword*x,int r,int c){
	if(fixed_y(y,x,r)/*true*/)fixed_x(*y,x,r,c);
}

static bool is_wordchar(char a){
	return is_word_char(a);
}
static bool is_textchar(char a){
	return a!='\t'&&a!=' '&&a!='('&&a!=')'&&a!=','&&a!='['&&a!=']'&&a!='{'
	&&a!='}';
}
static void left(WINDOW*w,int c){
	if(c>0)amove(w,getcury(w),c-1);
	else slmove(w,c,true);
}
static void left_move(WINDOW*w,bool(*f)(char)){
	int r=getcury(w);
	int c=getcurx(w);
	size_t y;row_dword x;
	fixed_yx(&y,&x,r,c);
	size_t sz=rows[y].sz;
	char*d=rows[y].data;
	if(x==sz||f(d[x])==false||x==0||f(d[x-1])==false){left(w,c);return;}
	size_t prevx=x;
	x--;
	for(;;){
		if(x==0)break;
		x--;
		if(f(d[x])==false){x++;break;}
	}
	if(x<xtext){xtext=x;refreshpage(w);c=0;}
	else c-=prevx-x;
	wmove(w,r,c);
}
#define left_wordmove(w) left_move(w,is_wordchar)
#define left_textmove(w) left_move(w,is_textchar)
static void right(WINDOW*w,int c){
	if(c+1<getmaxx(w))bmove(w,getcury(w),c+1,false);
	else srmove(w,c,false);
}
#define right_short(f,x,d,sz) x==sz||f(d[x])==false||x+1==sz||f(d[x+1])==false
static size_t right_long(size_t x,char*d,size_t sz,bool(*f)(char)){
	x++;
	for(;;){
		if(x+1==sz)break;
		x++;
		if(f(d[x])==false){x--;break;}
	}
	return x;
}
static void right_move(WINDOW*w,bool(*f)(char)){
	int r=getcury(w);
	int c=getcurx(w);
	size_t y;row_dword x;
	fixed_yx(&y,&x,r,c);
	size_t sz=rows[y].sz;
	char*d=rows[y].data;
	if(right_short(f,x,d,sz)){
		right(w,c);return;}
	size_t prevx=x;
	x=right_long(x,d,sz,f);
	c+=x-prevx;
	int max=getmaxx(w);
	if(c>=max){
		do{
			size_t val=d[xtext]=='\t'?tab_sz:1;
			xtext++;c-=val;
		}while(c>=max);
		refreshpage(w);
	}
	wmove(w,r,c);
}

#define right_wordmove(w) right_move(w,is_wordchar)
#define right_textmove(w) right_move(w,is_textchar)
#define alt_jump 2
#define ctrl_jump 3

#define click (BUTTON1_CLICKED|BUTTON1_PRESSED|BUTTON1_DOUBLE_CLICKED|BUTTON1_TRIPLE_CLICKED \
|BUTTON2_CLICKED|BUTTON2_PRESSED|BUTTON2_DOUBLE_CLICKED|BUTTON2_TRIPLE_CLICKED \
|BUTTON3_CLICKED|BUTTON3_PRESSED|BUTTON3_DOUBLE_CLICKED|BUTTON3_TRIPLE_CLICKED)

//1resize,0diff key,-1processed
movement_char movment(int c,WINDOW*w){
	if(c==KEY_MOUSE){
		MEVENT e;
#ifndef OTHER_CURSES
		getmouse(&e);//==OK is when mousemask is 0, but then nothing at getch
#else
		nc_getmouse(&e);
#endif
		if((e.bstate&BUTTON4_PRESSED)!=0)vu1move(w,getcury(w));
		else
	#ifdef BUTTON5_PRESSED
		if((e.bstate&BUTTON5_PRESSED)!=0)
	#else
		if(e.bstate==0)     // at wheel down (libncurses5 at bionic)
	#endif
		vd1move(w,getcury(w));
		else if((e.bstate&click)!=0)amove(w,e.y-topspace,e.x-leftspace);//return -2;}
	}else if(c==KEY_LEFT)left(w,getcurx(w));
	else if(c==KEY_RIGHT)right(w,getcurx(w));
	else if(c==KEY_UP){
		int y=getcury(w);
		if(y>0)amove(w,y-1,getcurx(w));
		else vu1move(w,y);
	}else if(c==KEY_DOWN){
		int y=getcury(w);
		if(y+1<getmaxy(w))amove(w,y+1,getcurx(w));
		else vd1move(w,y);
	}else if(c==KEY_HOME){
		int y=getcury(w);
		if(xtext!=0){
			xtext=0;
			refreshpage(w);
		}
		wmove(w,y,0);
	}else if(c==KEY_END)endmov(w,false);
	else if(c==KEY_PPAGE){
		if(ytext!=0){
			int y=getcury(w);int x=getcurx(w);
			size_t my=(size_t)getmaxy(w);
			ytext=my>=ytext?0:ytext-my;
			refreshpage(w);
			amove(w,y,x);
		}
	}else if(c==KEY_NPAGE){
		if(ytext<rows_tot-1){
			int y=getcury(w);int x=getcurx(w);
			ytext+=(size_t)getmaxy(w);
			if(ytext>rows_tot-1)ytext=rows_tot-1;
			refreshpage(w);
			amove(w,y,x);
		}
	}else if(c==KEY_SLEFT)slmove(w,getcurx(w),false);
	else if(c==KEY_SRIGHT)srmove(w,getcurx(w),true);
	else if(c==KEY_SHOME){
		int y=getcury(w);
		int x=getcurx(w);
		if(xtext!=0){
			xtext=0;
			refreshpage(w);
		}
		amove(w,y,x);
	}else if(c==KEY_SEND){
		int y=getcury(w);
		int x=getcurx(w);
		size_t r=ytext+(size_t)y;
		row_dword xcare=xtext;
		if(r<rows_tot)endmv(w,r,false);
		else xtext=0;
		if(xtext!=xcare)refreshpage(w);
		amove(w,y,x);
	}else if(c==KEY_SR)vu1move(w,getcury(w));
	else if(c==KEY_SF)vd1move(w,getcury(w));
	else if(c==KEY_RESIZE){
		return movement_resize;
	}
	else{
		const char*s=keyname(c);
		if(strcmp(s,"kLFT5")==0)left_textmove(w);
		else if(strcmp(s,"kRIT5")==0)right_textmove(w);
		else if(strcmp(s,"kLFT3")==0)left_wordmove(w);
		else if(strcmp(s,"kRIT3")==0)right_wordmove(w);
		else if(strcmp(s,"kHOM3")==0){
			int y=getcury(w);
			size_t r=ytext+(size_t)y;
			row_dword xcare=xtext;
			int x;if(r<rows_tot)x=home(w,r);
			else{xtext=0;x=0;}
			if(xcare!=xtext)refreshpage(w);
			wmove(w,y,x);
		}else if(strcmp(s,"kEND3")==0)endmov(w,true);
		else if(strcmp(s,"kUP5")==0)vumove(w,ctrl_jump);
		else if(strcmp(s,"kDN5")==0)vdmove(w,ctrl_jump);
		else if(strcmp(s,"kUP3")==0)vumove(w,alt_jump);
		else if(strcmp(s,"kDN3")==0)vdmove(w,alt_jump);
		else if(strcmp(s,"kHOM5")==0){
			if(ytext!=0||xtext!=0){
				ytext=0;xtext=0;
				refreshpage(w);
			}
			wmove(w,0,0);
		}else if(strcmp(s,"kEND5")==0){
			bool ycare;row_dword xcare=xtext;
			int val=(int)rows_tot-getmaxy(w);
			if((int)ytext<val){ytext=(size_t)val;ycare=true;}
			else ycare=false;
			size_t a=rows_tot-1;
			int y=(int)(a-ytext);
			int x=end(w,a);
			if(ycare||xtext!=xcare)refreshpage(w);
			//moved by curses, but no add str for line breaks
			wmove(w,y,x);
		}else if(strcmp(s,"kUP6")==0)vuNmove(w,getcury(w),ctrl_jump);
		else if(strcmp(s,"kDN6")==0)vdNmove(w,getcury(w),ctrl_jump);
		else if(strcmp(s,"kUP4")==0)vuNmove(w,getcury(w),alt_jump);
		else if(strcmp(s,"kDN4")==0)vdNmove(w,getcury(w),alt_jump);
		else return movement_diffkey;
	}
	return movement_processed;
}
size_t sizemembuf(size_t ybsel,row_dword xbsel,size_t yesel,row_dword xesel){
	if(ybsel==yesel)return xesel-xbsel;
	size_t size=rows[ybsel].sz-xbsel+ln_term_sz;
	for(size_t i=ybsel+1;i<yesel;i++){
		size+=rows[i].sz+ln_term_sz;
	}
	return size+xesel;
}
void cpymembuf(size_t ybsel,row_dword xbsel,size_t yesel,row_dword xesel,char*buf){
	row*b=&rows[ybsel];
	if(ybsel==yesel){
		memcpy(buf,b->data+xbsel,xesel-xbsel);
		return;
	}
	row_dword sz1=b->sz-xbsel;
	memcpy(buf,b->data+xbsel,sz1);
	memcpy(buf+sz1,ln_term,ln_term_sz);
	row_dword sz=sz1+ln_term_sz;
	for(size_t i=ybsel+1;i<yesel;i++){
		row_dword s=rows[i].sz;
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
	row_dword size=sizemembuf(ybsel,xbsel,yesel,xesel);
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
		row_dword sz=rows[y].sz;
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
		size_t ok=(size_t)xtext+getmaxx(w)-1;
		if(ok<rows[y].sz)return rows[y].sz;
		return ok;
	}else if(y==rmax){
		row_dword sz=rows[y].sz;
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
					int max=getmaxx(w)-1;
					if(rows[yb[0]].sz<(size_t)xtext+max)xb[0]=(size_t)xtext+max;
				}
			}
			ye[0]=yb[0];yb[0]=y;
			xe[0]=xb[0];xb[0]=x;
			orig[0]=false;
		}
		else if(y>yb[0]||xb[0]<=x){
			if(v_l/*true*/)x=v_l_x(y,x,rmax,w);
			else if(y<rmax&&x>=rows[y].sz)x=(size_t)xtext+getmaxx(w)-1;
			ye[0]=y;xe[0]=x;
		}
		else{
			if(yb[0]<rmax&&xb[0]==rows[yb[0]].sz){
				size_t max=(size_t)getmaxx(w)-1;
				if(rows[yb[0]].sz<(size_t)xtext+max)xb[0]=(size_t)xtext+max;
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
				if(y<rmax&&x>=rows[y].sz)x=(size_t)xtext+getmaxx(w)-1;}
			yb[0]=ye[0];ye[0]=y;
			xb[0]=xe[0];xe[0]=x;
			orig[0]=true;
		}
		else if(ye[0]>y){
			if(v_l/*true*/){if(y<=rmax)x=0;}
			else if(y<rmax&&x>rows[y].sz)x=rows[y].sz;
			if(ye[0]<rmax&&xe[0]>=rows[ye[0]].sz){
				int max=getmaxx(w)-1;
				if(rows[ye[0]].sz<(size_t)xtext+max)xe[0]=(size_t)xtext+max;
			}
			yb[0]=y;xb[0]=x;
		}
		else if(xe[0]<x){
			if(y<rmax&&x>=rows[y].sz)x=(size_t)xtext+getmaxx(w)-1;
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
static void unsel(WINDOW*w){
	sel(w,color_0,color_0,_rb,_cb,_re,_ce);
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
	if(b/*true*/)sel(w,color_a,color_b,rb,cb,re,ce);
}
static void printsel(WINDOW*w,size_t ybsel,size_t xbsel,size_t yesel,size_t xesel,bool care){
	int wd=getmaxx(w);
	int cright=wd-1;
	int rb;int cb;
	if(ybsel<ytext){
		rb=0;cb=0;}
	else{
		rb=(int)(ybsel-ytext);
		if(xbsel<=xtext)cb=0;
		else if(xbsel<=(size_t)xtext+c_to_xc(cright,rb))cb=xc_to_c(xbsel-xtext,rb);
		else{rb++;cb=0;}
	}
	int re;int ce;
	int rdown=getmaxy(w)-1;
	size_t ydown=ytext+(size_t)rdown;
	if(yesel>ydown){
		re=rdown;ce=cright;}
	else{
		re=(int)(yesel-ytext);
		if((size_t)xtext+c_to_xc(cright,re)<=xesel)ce=cright;
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
	if(care/*true*/)sel(w,color_a,color_b,rb,cb,re,ce);
	else difsel(w,rb,cb,re,ce);
	wattrset(w,0);
	_rb=rb;_cb=cb;
	_re=re;_ce=ce;
}
static void refreshrowscond(WINDOW*w,size_t y,row_dword x,size_t r,size_t n){
	if(y!=ytext||x!=xtext)refreshpage(w);
	else refreshrowsbot(w,(int)r,n!=0?getmaxy(w):(int)r+1);
}
static void pasted(size_t r,size_t x,WINDOW*w){
	size_t z1=ytext;row_dword z2=xtext;size_t z3=r;
	size_t rws=cutbuf_r-1;
	r+=rws;int maxy=getmaxy(w);
	if(maxy<=r){ //to silence rpmbuild warning, unsigned
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
		xtext=(s-d);
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
		if(d<text_init_b||text_init_e<=d)free(d);//the check is if the text was not modified from the start in this case will be free at exit(all initial lines)
	}
}
static row_dword row_pad_sz(row_dword sz){
	sz++;//[i]=0;addstr;=aux
	char dif=sz&row_pad;
	if(dif!=0)return sz+((dif^row_pad)+1);
	return sz;
}
bool row_alloc(row*rw,row_dword l,size_t c,row_dword r){//c at pasting is coming from cutbuf
	size_t sz=l+c+r;
	if(sz>=rw->spc){//[i]=0;addstr;=aux
		char*src=rw->data;char*dst;
		row_dword size=row_pad_sz(sz);
		if(sz>size)return true;//this and another safety like this, for segmentations
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
void row_set(row*rw,row_dword l,row_dword c,row_dword r,const char*mid){
	char*d=rw->data;
	size_t j=l+c;size_t i=j+r;row_dword k=rw->sz;
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
static void row_del(size_t a,size_t b,int r){
	size_t c=b+1;
	text_free(a,c);
	row*j=&rows[a];
	for(size_t i=c;i<rows_tot;i++){
		memcpy(j,&rows[i],sizeof(row));
		j++;
	}
	size_t diff=c-a;
	rows_tot-=diff;
	if(ocompiler_flag/*true*/){
		if(aftercall<(a-1)){}//example deleting first new line aftercall=0 a=1 b=1
		else if(b<aftercall)aftercall-=diff;
		else if(b==aftercall){
			aftercall-=diff;
			if(r!=-1)printsyntax(color_d,r);//at backspace when moving one row up
			//this r -1 is from backspacing first row on screen
		}
		else{
			aftercall=rows_tot;//will be redrawn in the next functions
			//on select all will be ok
			//on backspace,delete need to set the current row, anyway on select is not fast without r=-1
			if(r!=-1)printsyntax(color_0,r);
		}
	}
}

static void easytime(){
	hardtime=0;
}
static void mod_visual(chtype ch){
	visual_write_at(ch,1);
	wnoutrefresh(stdscr);
}
static void mod_set(bool flag,chtype ch){
	mod_flag=flag;
	mod_visual(ch);
}
#define modif_visual '*'
void mod_set_off(){
	hardtime=time(nullptr);//(time_t*)
	//cannot delete mod_flag, it has meanings at undo type/bk/del sum and quit without save
	mod_set(false,modif_visual);
}
void mod_set_on(){
	easytime();
	mod_set(true,' ');
}
void mod_set_off_wrap(){
	//if(mod_flag/*true*/){
	if(hardtime==0){
		mod_set_off();//with wrap
	}
}

#define restore_marker ".edorrestorefile"
#define restorefile_path(a) restorefile_path_base(a,restorefile_buf)
static bool restorefile_path_base(char*p,char*dest){
	size_t ln=strlen(p)+sizeof(restore_marker);
	if(ln>max_path_0)return false;//the path is too long
	sprintf(dest,"%s%s",p,restore_marker);
	return true;
}
#define editing_marker ".edoreditingfile"
#define editingfile_path(a) editingfile_path_base(a,editingfile_buf)
static bool editingfile_path_base(char*p,char*dest){
	size_t ln=strlen(p)+sizeof(editing_marker);
	if(ln>max_path_0)return false;//the path is too long
	sprintf(dest,"%s%s",p,editing_marker);
	return true;
}
static void editing_new(){
	int f=open_new(editingfile_buf);
	if(f!=-1){
		close(f);
		editingfile=editingfile_buf;
	}
}

static void restore_visual(){
	mod_visual('&');
}
static void hardtime_resolve_returner(WINDOW*w){//argument for errors
	if(textfile!=nullptr){
		if(restorefile==nullptr){
			//set restore file path
			if(restorefile_path(textfile)/*true*/){
				//save at path
				if(saving_base(restorefile_buf)==command_ok){
					restorefile=restorefile_buf;
					restore_visual();
				}else err_set(w);
			}
		}else{
			if(saving_base(restorefile)==command_ok)restore_visual();
			else err_set(w);
		}
	}
}
#define one_minute 60
#define one_minute_less one_minute-1
//#define one_minute 3
static void hardtime_resolve(WINDOW*w){//argument for errors
	if(hardtime!=0){
		//if((time((time_t)nullptr)-hardtime)>=one_minute){//= here is easily tested(set to 3, will wait 6 seconds if not and equal
		if((time(nullptr)-hardtime)>one_minute_less){//(time_t*)
			hardtime_resolve_returner(w);
			easytime();
		}
	}
}
//rename is better than delete+create, new disk cycles?
#define file_rebase(file,s,d,call)\
	char*src;char*dest;\
	if(file==s){\
		src=s;dest=d;\
	}else{\
		src=d;dest=s;\
	}\
	if(call(textfile,dest)/*true*/){\
		if(rename(src,dest)==0)file=dest;\
	}
void restore_rebase(){
	if(restorefile!=nullptr){
		file_rebase(restorefile,restorefile_buf,restorefile_buf2,restorefile_path_base)
	}
}
void editing_rebase(){
	if(editingfile!=nullptr){
		file_rebase(editingfile,editingfile_buf,editingfile_buf2,editingfile_path_base)
	}else if(editingfile_path(textfile)/*true*/)editing_new();
}

void deleting(size_t ybsel,size_t xbsel,size_t yesel,size_t xesel){
	row*r1=&rows[ybsel];
	if(ybsel==yesel){
		row_dword sz=r1->sz;
		size_t dif=xesel-xbsel;
		char*d=rows[ybsel].data;
		for(size_t i=xesel;i<sz;i++){
			d[i-dif]=d[i];
		}
		r1->sz-=dif;
	}else{
		row_set(r1,xbsel,rows[yesel].sz-xesel,0,rows[yesel].data+xesel);
		row_del(ybsel+1,yesel,-1);
	}
}
bool deleting_init(size_t ybsel,row_dword xbsel,size_t yesel,row_dword xesel){
	if(ybsel!=yesel){
		row_dword se=rows[yesel].sz-xesel;
		row_dword sb=rows[ybsel].sz-xbsel;
		if(se>sb){//need deleting part at undo
			return row_alloc(&rows[ybsel],rows[ybsel].sz,se-sb,0);
		}
	}
	return false;
}
static bool deletin(size_t ybsel,row_dword xbsel,size_t yesel,row_dword xesel,int*rw,int*cl,WINDOW*w,bool many){
	if(deleting_init(ybsel,xbsel,yesel,xesel)==false){
		if(undo_add_del(ybsel,xbsel,yesel,xesel)==false){
			deleting(ybsel,xbsel,yesel,xesel);
			size_t z1=ytext;row_dword z2=xtext;
			deleted(ybsel,xbsel,rw,cl,w);
			refreshrowscond(w,z1,z2,ybsel-ytext,many);
			return true;
		}
	}
	return false;
}
static bool deleti(size_t ybsel,size_t xbsel,size_t yesel,size_t xesel,int*rw,int*cl,WINDOW*w){
	fixmembuf(&ybsel,&xbsel);
	fixmembuf(&yesel,&xesel);
	if(xesel==rows[yesel].sz){if(yesel<rows_tot-1){yesel++;xesel=0;}}
	else xesel++;
	bool many=ybsel!=yesel;
	if(many/*true*/||xbsel!=xesel){
		if(deletin(ybsel,xbsel,yesel,xesel,rw,cl,w,many)/*true*/){
			mod_set_off_wrap();
			return true;
		}
	}
	return false;
}
static bool delet(size_t ybsel,size_t xbsel,size_t yesel,size_t xesel,int*rw,int*cl,WINDOW*w){
	size_t yend=yesel;
	size_t rend=rows_tot;
	size_t wasy=ytext;
	row_dword wasx=xtext;
	bool b=deleti(ybsel,xbsel,yesel,xesel,rw,cl,w);
	//unselect all
	if(yend>=rend&&wasy==ytext&&wasx==xtext){
		rend-=ytext;
		size_t ymax=(size_t)getmaxy(w);
		if(rend<ymax){
			yend-=ytext;
			size_t to=yend<ymax?yend:ymax-1;
			while(rend<=to){
				wmove(w,(int)rend,0);wclrtoeol(w);rend++;
			}
		}
	}
	return b;
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
	if(ocompiler_flag/*true*/){
		if(aftercall>=(off-1))aftercall+=sz;  //example enter at 0,0 off will be 1
	}
}
static size_t pasting(row*d,size_t y,row_dword x,row_dword*xe,char*buf,size_t buf_sz,size_t buf_r,bool fromcopy){
	bool one=buf_r==1;
	size_t szc;size_t l;
	row_dword sz1r;row_dword szr=rows[y].sz-x;
	size_t max=buf_r-1;
	if(one/*true*/){
		szc=buf_sz;sz1r=szr;
		l=x+szc;
	}
	else{
		char*a=memtrm(buf)+ln_term_sz;
		sz1r=0;
		size_t sz=a-buf;
		szc=sz-ln_term_sz;
		size_t n=max-1;
		//inter
		for(size_t i=0;i<n;i++){
			char*b=memtrm(a)+ln_term_sz;
			size_t ln=b-a;
			row_dword len=ln-ln_term_sz;//this will not stop at something big but will trunc
			row_dword spc_sz=row_pad_sz(len);
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
		row_dword spc_sz=row_pad_sz(sizen);
		if(sizen>spc_sz)return max;//here something big will stop(example at a big cutbuf)
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
bool paste(size_t y,row_dword x,row_dword*xe,char*buf,size_t buf_sz,size_t buf_r,bool fromcopy){
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
		size_t y;row_dword x;
		fixed_yx(&y,&x,r,getcurx(w));
		row_dword xe;
		if(paste(y,x,&xe,cutbuf,cutbuf_sz,cutbuf_r,true)/*true*/){
			pasted(y-ytext,xe,w);
			mod_set_off_wrap();
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
					c++;
					if(c==max){
						break;
					}
				}
				k+=j;
			}
			else{mapsel[k]=no_char(ch)/*true*/?'?':ch;c++;k++;}
			x++;//before c==max for 'at right'
			if(c==max){
				//can test 'at right' here but when deleting a tab c can be less than max with tab - 1
				break;
			}
		}
		if(x==sz)no_content_at_right(r);
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
	row_dword mx=rw->sz;
	while(i<mx){
		char ch=d[i];
		if(ch!='\t'){
			c++;waddch(w,no_char(ch)/*true*/?'?':ch);
		}else{
			t[a]=c;t[0]++;a++;
			c+=tab_sz;wmove(w,r,c);
		}
		i++;
		if(c>=wd){ //here == is not ok, if c+=tab_sz goes > then will be wrong (also, visible on display)
			if(i<mx)content_at_right(r);
			break;
		}
	}
	x_right[r]=mx!=0;
}
static bool del_key(size_t y,row_dword x,int r,int*cc,WINDOW*w,bool reverse){
	row*r1=&rows[y];int maxx=getmaxx(w);int c=*cc;
	int margin_val=c+view_margin;
	bool margin=margin_val>maxx;
	if(margin/*true*/){while(c+view_margin>maxx){
			c-=r1->data[xtext]=='\t'?tab_sz:1;
			xtext++;
		}
		refreshpage(w);*cc=c;
	}
	row_dword sz=r1->sz;
	if(x==sz){
		size_t yy=y+1;
		if(yy==rows_tot){
			//to not continue
			if(margin/*true*/)wmove(w,r,c);//this is after refreshpage, the pointer is at the last row
			return true;
		}
		row*r2=&rows[yy];
		if(row_alloc(r1,x,r2->sz,0)==false){
			if(undo_add_del(y,x,yy,0)==false){
				row_set(r1,x,r2->sz,0,r2->data);
				row_del(yy,yy,r);
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
		if(margin/*true*/)wmove(w,r,c);
		else if(reverse/*true*/&&xtext>0&&margin_val<maxx){
			xtext--;refreshpage(w);*cc=c+1;
			return false;
		}
		delete_fast(w,r,c,data,x,r1->sz);
		return false;
	}
	return true;
}
#define delete_key(y,x,r,cc,w) del_key(y,x,r,cc,w,false)
static bool bcsp(size_t y,row_dword x,int*rw,int*cl,WINDOW*w){
	int c=cl[0];
	if(xtext==0&&c==0){
		if(y==0)return true;//to not continue
		size_t yy=y-1;
		row*r0=&rows[yy];
		row*r1=&rows[y];
		row_dword sz0=r0->sz;
		row_dword xx=xtext;c=end(w,yy);
		if(row_alloc(r0,sz0,r1->sz,0)==false){
			if(undo_add_del(yy,sz0,y,0)==false){
				row_set(r0,sz0,r1->sz,0,r1->data);
				int r=rw[0];
				row_del(y,y,r-1);
				cl[0]=c;
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
		char*data=r->data;row_dword sz=r->sz;
		c-=data[x-1]=='\t'?tab_sz:1;
		for(size_t i=x;i<sz;i++){
			data[i-1]=data[i];
		}
		r->sz--;
		if(xtext!=0){
			if(c<view_margin){
				if(c<0){c=0;xtext--;}
				if(xtext!=0){
					while(c<view_margin){
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
static bool enter(size_t y,row_dword x,int*r,int*c,WINDOW*w){
	if(rows_expand(1)/*true*/)return true;
	char*b=rows[y].data;
	char*d=b;
	if(indent_flag/*true*/){
		char*e=b+x;
		while(d<e&&d[0]=='\t')d++;
	}
	row_dword tb=d-b;
	row_dword s=rows[y].sz-x;
	row_dword sze=tb+s;
	row_dword spc=row_pad_sz(sze);
	char*v=(char*)malloc(spc);
	if(v==nullptr)return true;
	if(undo_add(y,x,y+1,tb)==false){
		row rw;
		memset(v,'\t',tb);//tb can be 0
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
				c[0]=(tb-xtext)*tab_sz;

				wclrtoeol(w);
				no_content_at_right_if(row)
				x_right[row]=xtext<rows[y].sz;

				refreshrows(w,row+1);
				return false;
			}
		}
		if(fix/*true*/)c[0]=(tb-xtext)*tab_sz;
		else{xtext=tb;c[0]=0;}
		refreshpage(w);
		return false;
	}
	return true;
}
#define multidel(fn,r,x,y,cl,rw,w)\
	char*d=r->data;row_dword sz=r->sz;\
	if(right_short(fn,x,d,sz)){if(delete_key(y,x,rw,&cl,w)/*true*/)return;}\
	else if(deletin(y,x,y,right_long(x,d,sz,fn)+1,&rw,&cl,w,false)==false)return;
static void type(int cr,WINDOW*w){
	int cl=getcurx(w);
	int rwnr=getcury(w);
	size_t x=(size_t)xtext+c_to_xc(cl,rwnr);
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
	else if(is_KEY_BACKSPACE(cr)){
		if(bcsp(y,x,&rw,&cl,w)/*true*/)return;
		position(rw,cl);
	}
	else if(cr==KEY_DC){if(delete_key(y,x,rw,&cl,w)/*true*/)return;}
	else if(cr==KEY_SDC){if(del_key(y,x,rw,&cl,w,true)/*true*/)return;}//shift+delete example: xtext>0 and at center, will move the page not the right text
	else{
		const char*knm=keyname(cr);
		if(strcmp(knm,"kDC5")==0){multidel(is_textchar,r,x,y,cl,rw,w)}
		else if(strcmp(knm,"kDC3")==0){multidel(is_wordchar,r,x,y,cl,rw,w)}
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
				wmove(w,rw,colmn);//if cursor was adjusted and page was refreshed
				int n=max-cl;
				winnstr(w,mapsel,n);
				int*t=&tabs[tabs_rsz*rw];
				int a=t[0];

				if(a!=0)if((t[a]+s)>=max){t[0]--;a--;}

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
				char marker=(char)mvwinch(rightcontent,rw,0);
				if(marker!=at_right_mark){
					int newright=xc_to_c(r->sz-xtext-1,rw);
					if(newright>=max)content_at_right(rw);
				}
				x_right[rw]=true;
			}
			position(rw,cl);
		}
	}
	wmove(w,rw,cl);
	mod_set_off_wrap();
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
		mod_set_off_wrap();
	}else{
		bool something=false;
		for(size_t i=ybsel;i<=ye;i++){
			if(rows[i].sz!=0){something=true;break;}
		}
		if(something/*true*/){
			if(undo_add_ind_del(ybsel,ye)/*true*/)return;
			for(size_t i=ybsel;i<ye;i++){
				row*r=&rows[i];row_dword sz=r->sz;
				if(sz!=0){
					char*d=r->data;
					for(size_t j=1;j<sz;j++)d[j-1]=d[j];
					r->sz=sz-1;
				}
			}
			mod_set_off_wrap();
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
			if(xtext==0){//add at left markers for content
				for(int i=rb;i<re;i++)mvwaddch(leftcontent,i,0,at_left_mark);
				wnoutrefresh(leftcontent);
			}
			xtext++;
			if(rb!=0)refreshrowsbot(w,0,rb);
			if(re<max)refreshrowsbot(w,re,max);
		}else refreshrowsbot(w,rb,re);
	}else{
		if(xbsel!=nullptr){
			if(xtext!=0){
				xbsel[0]--;xesel[0]--;
				xtext--;
				if(xtext==0){
					if(xtext==0){//add at left markers for content
						for(int i=rb;i<re;i++)mvwaddch(leftcontent,i,0,at_content_nomark);
						wnoutrefresh(leftcontent);
					}
				}
				if(rb!=0)refreshrowsbot(w,0,rb);
				if(re<max)refreshrowsbot(w,re,max);
			}else{
				xbsel[0]=0;xesel[0]=v_l_x(yesel,xesel[0],rows_tot-1,w);
				refreshrowsbot(w,rb,re);
				printsel(w,ybsel,0,yesel,xesel[0],true);
			}
		}else refreshrowsbot(w,rb,re);
	}
}
//true resize
static bool visual_mode(WINDOW*w,bool v_l){
	visual(orig_upkey(key_visual));
	int rw=getcury(w);int cl=getcurx(w);
	size_t ybsel=ytext+(size_t)rw;
	size_t yesel=ybsel;
	size_t xbsel;size_t xesel;
	bool orig=true;
	size_t rmax=rows_tot-1;
	if(v_l/*true*/){
		if(ybsel<rmax){
			xbsel=0;
			xesel=(size_t)xtext+getmaxx(w)-1;
			if(xesel<rows[yesel].sz)xesel=rows[yesel].sz;
		}
		else{
			xesel=(size_t)xtext+cl;
			if(ybsel==rmax){
				xbsel=0;
				size_t sz=rows[yesel].sz;
				if(sz!=0&&xesel<sz-1)xesel=sz-1;
			}else xbsel=xesel;
		}
	}else{
		xbsel=(size_t)xtext+c_to_xc(cl,rw);xesel=xbsel;
		if(yesel<rmax&&xesel>=rows[yesel].sz){
			xbsel=rows[ybsel].sz;
			xesel=(size_t)xtext+getmaxx(w)-1;
		}
	}
	printsel(w,ybsel,xbsel,yesel,xesel,true);
	wmove(w,rw,cl);
	movement_char z;
	do{
		int b=wgetch(w);
		size_t ycare=ytext;row_dword xcare=xtext;
		z=movment(b,w);
		if(z==movement_resize)return true;
		else{
			int r=getcury(w);int c=getcurx(w);
			if(z==movement_diffkey){
				if(b=='I'){z=-1;indent(true,ybsel,&xbsel,yesel,&xesel,w);}
				else if(b=='U'){
					z=-1;
					bool edge=xtext==0;
					indent(false,ybsel,&xbsel,yesel,&xesel,w);
					if(edge/*true*/){amove(w,r,c);continue;}
				}
				else{
					char v=' ';
					//visual_bool=b=='c';
					//if(visual_bool/*true*/){
					if(b=='c'){
						if(writemembuf(ybsel,xbsel,yesel,xesel)/*true*/){v='C';unsel(w);}
					}else if(b=='d'){
						if(delet(ybsel,xbsel,yesel,xesel,&r,&c,w)/*true*/)
							if(orig/*true*/)position(r,c);
					}else if(b=='x'){
						if(writemembuf(ybsel,xbsel,yesel,xesel)/*true*/){
							if(delet(ybsel,xbsel,yesel,xesel,&r,&c,w)/*true*/)
								if(orig/*true*/)position(r,c);
						}
					}else{
						if(b=='i'){
							indent(true,ybsel,nullptr,yesel,nullptr,w);
							amove(w,r,c);
							c=getcurx(w);
						}else if(b=='u'){
							indent(false,ybsel,nullptr,yesel,nullptr,w);
							amove(w,r,c);
							c=getcurx(w);
						}
						unsel(w);
					}
					visual(v);
				}
			}else{//movement_processed
				position(getcury(w),getcurx(w));
				size_t y=ytext+(size_t)r;size_t x=(size_t)xtext+c_to_xc(c,r);
				setmembuf(y,x,&orig,&ybsel,&xbsel,&yesel,&xesel,w,v_l);
				printsel(w,ybsel,xbsel,yesel,xesel,ytext!=ycare||xtext!=xcare);
			}
			wmove(w,r,c);
		}
	}while(z!=movement_diffkey);
	return false;
}
#define quick_pack(nr,w) comnrp_define args[2];((comnrp_define)args)[0]=nr;args[1]=(comnrp_define)w;
#define quick_pack3(nr,fn,w) comnrp_define args[3];((comnrp_define)args)[0]=nr;args[1]=(comnrp_define)fn;args[2]=(comnrp_define)w;
static bool find_mode(int nr,WINDOW*w){
	quick_pack((long)nr,w)
	command_char r=command((comnrp_define)args,(show_key_struct){key_find,0});
	if(r==command_resize)return true;
	//if(r==command_no){//only at quit from the bar, but at command_ok also to update stdscr, and at command_false visib<2 at finds
	//	only at first command_false visib<2 is extra, to find that will be extra in extra
	wmove(w,getcury(w),getcurx(w));
	//}
	return false;
}
static bool goto_mode(char*args,WINDOW*w){
	command_char r=command(args,(show_key_struct){key_goto,0});
	if(r==command_ok){
		centering_simple(w)
	}
	else if(r>command_resize)wmove(w,getcury(w),getcurx(w));//-1 quit and 0 nothing
	return true;//-2
	//return false;
}
static bool savetofile(WINDOW*w,bool has_file){
	char*d=textfile;
	command_char ret;
	if(has_file){
		ret=save();
	}else{char aa=com_nr_save;ret=command(&aa,(show_key_struct){key_save,0});}
	if(ret!=command_false){
		if(ret==command_ok){
			if(d!=textfile){
				//text_file=textfile;//now is a title
				topspace_clear();
				write_title();
			}

			if(mod_flag/*true*/){
				bar_clear();texter_macro("Saved");
				//there are some cases here:
				//	open with forced new line and save
				//	open with std input and save
				//	save a blank New Path
				//	just save in case the file was erased
				wnoutrefresh(stdscr);
			}else{//attention to not write Saved and blind clean the * because curses will also clean the position. because is another window?
				mod_set(true,' ');
			}

			easytime();
			undo_save();
		}
		else if(ret==command_resize)return true;
	}else err_set(w);
	wmove(w,getcury(w),getcurx(w));
	return false;
}
static void writeprefs(int f,char mask){
	if(write(f,&mask,mask_size)==mask_size){
		bar_byte sz=strlen(ocode_extension);//at prefs one byte is taken, and also input has 255 max
		if(write(f,&sz,extlen_size)==extlen_size){
			if(write(f,ocode_extension,sz)==sz){
				if(split_writeprefs(f)/*true*/){
					char k=0;
					if(memcmp(keys_row,keys_row_orig,number_of_keys)!=0)k=number_of_keys;
					if(write(f,&k,sizeof(char))==sizeof(char)){
						if(write(f,keys_row,k)==k){
							sz=strlen(filewhites_extension);
							if(write(f,&sz,extlen_size)==extlen_size){
								if(write(f,filewhites_extension,sz)==sz){
									#pragma GCC diagnostic push
									#pragma GCC diagnostic ignored "-Wunused-result"
									write(f,&tab_sz,sizeof(tab_protocol));
									#pragma GCC diagnostic pop
								}
							}
						}
					}
				}
			}
		}
	}
}
void setprefs(int flag,bool set){
	if(prefs_file[0]!='\0'){
		int f=open(prefs_file,O_RDWR|O_CREAT,S_IRUSR|S_IWUSR);
		if(f!=-1){
			char mask;
			if(read(f,&mask,mask_size)!=mask_size)mask=mask_nomask;
			else lseek(f,0,SEEK_SET);
			if(flag!=(mask_nomask)){
				if(set/*true*/)mask|=flag;
				else mask&=~flag;
			}
			writeprefs(f,mask);
			unsigned short nowsize=lseek(f,0,SEEK_CUR);//now is 7 bar_bytes and a mask, not 256
			#pragma GCC diagnostic push
			#pragma GCC diagnostic ignored "-Wunused-result"
			ftruncate(f,nowsize);
			#pragma GCC diagnostic pop
			close(f);
		}
	}
}

void pref_modify(char**pref_orig,char**pref_buf,bool sizedonly,char*newinput,bar_byte cursor){
	if(cursor==0)if(sizedonly/*true*/)return;
	if(*pref_buf!=nullptr){
		size_t len=strlen(*pref_buf);
		if(len<cursor){
			char*newmem=(char*)malloc(cursor+1);
			if(newmem!=nullptr)*pref_buf=newmem;
			else return;
		}
	}else{
		char*newmem=(char*)malloc(cursor+1);
		if(newmem!=nullptr)*pref_buf=newmem;
		else return;
	}
	memcpy(*pref_buf,newinput,cursor);
	(*pref_buf)[cursor]='\0';
	*pref_orig=*pref_buf;//at start extension_new is not 100%
	rewriteprefs;
}
static bool pref_change(WINDOW*w,char**pref_orig,char**pref_buf,bool sizedonly,char k,char add){
	extdata d={pref_orig,pref_buf,sizedonly};
	quick_pack(com_nr_ext,&d)
	command_char nr=command((char*)args,(show_key_struct){k,add});
	if(nr>command_resize){
		wmove(w,getcury(w),getcurx(w));//ok/quit/err
		return false;
	}
	return true;
}

void changekey(unsigned char i){
	key_struct*k=&keys[i];
	char newkey=i+_0_to_A;
	*(k->key_location)=newkey;
	char pos_total=k->pos_total;
	if(pos_total!=0){
		unsigned short* pos=k->pos;
		for(int j=0;j<pos_total;j++){
			keys_helptext[pos[j]]=newkey+A_to_a;
		}
	}
	unsigned short upos=k->upos;
	if(upos!=0){
		keys_helptext[upos]=newkey;
	}
}
static bool quit_from_key(WINDOW*w,bool *b){
	if(mod_flag==false){
		bar_clear();//errors
		command_char q=question("And save");
		if(q==command_ok){
			q=save();
			if(q==command_false)err_set(w);
		}
		if(q==command_resize){*b=true;return false;}
		else if(q==command_false){
			wnoutrefresh(stdscr);
			wmove(w,getcury(w),getcurx(w));
			return true;
		}
	}
	if(restorefile!=nullptr)unlink(restorefile);//here restorefile is deleted
	*b=false;return false;
}

static time_t guardian=0;
static bool loopin(WINDOW*w){
	int c;
	for(;;){
		//wtimeout(w,1000);
		wtimeout(w,one_minute*1000);//it counts where wgetch is (example at visual)
		c=wgetch(w);
		hardtime_resolve(w);
		if(c==ERR){
			time_t test=time(nullptr);
			if(test==guardian)return false;//example: cc nothing.c | edor , will have errno 0, will loop.
				//reproducible? fprintf to stderr+something else, see cc source for answer
				//fprintf was tested and is separate from this, then why? at that cc is showing same time with edor
			guardian=test;

			//this was ok at hardtime_resolve but will be too often there, here will be wrong sometimes but still less trouble
			//doupdate();//noone will show virtual screen if without this

			//and the cursor is getting away, not right but ok
			wmove(w,getcury(w),getcurx(w));
			//same as doupdate+not moving the cursor

			continue;//timeout
		}
		wtimeout(w,-1);

		movement_char a=movment(c,w);
		if(a==movement_resize)return true;
		if(a!=movement_diffkey){
			//if(visual_bool/*true*/){//here only when C/S at visual
			//	visual_bool=false;
			//	visual(' ');
			//}
			//else
			if(bar_clear()/*true*/)wnoutrefresh(stdscr);
			position(getcury(w),getcurx(w));
		}else if(c==Char_Escape){
			nodelay(w,true);
			int z=wgetch(w);
			nodelay(w,false);
			int y;bool b;
			if(z==(key_visual+A_to_a)){if(visual_mode(w,true)/*true*/)return true;}
			else if(z==(key_paste+A_to_a)){
				y=getcury(w);
				if(xtext!=0){xtext=0;refreshpage(w);}
				wmove(w,y,0);past(w);
			}else if(z==(key_goto+A_to_a)){
				quick_pack3(com_nr_goto_alt,go_to,w)
				if(goto_mode((char*)args,w)/*true*/)return true;
			}else if(z==(key_find+A_to_a)){if(find_mode(com_nr_findagain,w)/*true*/)return true;}
			else if(z==(key_findword+A_to_a)){if(find_mode(com_nr_findwordfrom,w)/*true*/)return true;}
			else if(z==(key_undo+A_to_a)){vis('U',w);undo_loop(w);vis(' ',w);}
			else if(z==(key_save+A_to_a)){b=savetofile(w,false);if(b/*true*/)return true;}
			else if(z==(key_ocomp+A_to_a)){aftercall=aftercall_find();aftercall_draw(w);}
			else if(z==(key_actswf+A_to_a)){if(pref_change(w,&sdelimiter,&sdelimiter_new,true,key_actswf,A_to_a)/*true*/)return true;}//don't allow no size delimiters
			else if(z==(key_actswf2+A_to_a)){if(pref_change(w,&split_out,&split_out_new,false,key_actswf2,A_to_a)/*true*/)return true;}
			else if(z==key_whites+A_to_a){if(pref_change(w,&filewhites_extension,&filewhites_extension_new,false,key_whites,A_to_a)/*true*/)return true;}
			else if(z==key_quit+A_to_a){
				bool q;bool not_q=quit_from_key(w,&q);
				if(not_q/*true*/)continue;
				if(q==false)user_return=EXIT_FAILURE;
				return q;
			}
			else if(z==key_ocomp){if(pref_change(w,&ocode_extension,&ocode_extension_new,false,key_ocomp,0)/*true*/)return true;}
			else if(z==key_actswf){if(pref_change(w,&esdelimiter,&esdelimiter_new,true,key_actswf,0)/*true*/)return true;}            //don't allow no size delimiters
			else if(z==key_actswf2){if(pref_change(w,&split_extension,&split_extension_new,false,key_actswf2,0)/*true*/)return true;}
		}else{
			const char*s=keyname(c);
			if(*s==Char_Ctrl){//seems that all cases are ^ a letter \0
				char chr=s[1];
				if(chr==key_visual){
					if(visual_mode(w,false)/*true*/)return true;
				}else if(chr==key_paste)past(w);
				else if(chr==key_save){
					bool b=savetofile(w,true);
					if(b/*true*/)return true;
				}else if(chr==key_goto){
					quick_pack(com_nr_goto,go_to)
					if(goto_mode((char*)args,w)/*true*/)return true;
				}else if(chr==key_find){
					if(find_mode(com_nr_find,w)/*true*/)return true;
				}else if(chr==key_findword){
					if(find_mode(com_nr_findword,w)/*true*/)return true;
				}else if(chr==key_undo){
					undo(w);
				}else if(chr==key_redo){
					redo(w);
				}else if(chr==key_titles){
					if(titles(w)/*true*/)return true;
				}else if(chr==key_quit){
					bool q;bool not_q=quit_from_key(w,&q);
					if(not_q/*true*/)continue;
					return q;
				}else if(chr==key_insens){
					char c;
					if(insensitive/*true*/){insensitive=false;c=orig_lowkey(key_insens);}
					else{insensitive=true;c=orig_upkey(key_insens);}
					setprefs(mask_insensitive,insensitive);//here the bit is set on full insensitive search
					vis(c,w);//is not showing on stdscr without wnoutrefresh(thisWindow)
				}else if(chr==key_mouse){
					char c;
					if(stored_mouse_mask_q){stored_mouse_mask=mousemask(0,nullptr);c=orig_lowkey(key_mouse);setprefs(mask_mouse,false);}
					else{stored_mouse_mask=mousemask(ALL_MOUSE_EVENTS,nullptr);c=orig_upkey(key_mouse);setprefs(mask_mouse,true);}
					vis(c,w);
				}else if(chr==key_indents){
					char c;
					if(indent_flag/*true*/){indent_flag=false;c=orig_lowkey(key_indents);}
					else{indent_flag=true;c=orig_upkey(key_indents);}
					setprefs(mask_indent,indent_flag);
					vis(c,w);//is not showing on stdscr without wnoutrefresh(thisWindow)
				}else if(chr==key_ocomp){
					if(ocompiler_flag/*true*/){ocompiler_flag=false;c=orig_lowkey(key_ocomp);}
					else{ocompiler_flag=true;c=orig_upkey(key_ocomp);
						aftercall=aftercall_find();}
					setprefs(mask_ocompiler,ocompiler_flag);
					visual(c);//addch for more info, first to window, then wnoutrefresh to virtual, then doupdate to phisical
					aftercall_draw(w);
				}else if(chr==key_actswf){//joins //alt j is set delimiter,alt J escape delimiter
					char c;
					if(splits_flag/*true*/){splits_flag=false;c=orig_lowkey(key_actswf);}
					else{splits_flag=true;c=orig_upkey(key_actswf);}
					setprefs(mask_splits,splits_flag);
					vis(c,w);
				}else if(chr==key_whites){
					char c;
					if(filewhites_flag/*true*/){filewhites_flag=false;c=orig_lowkey(key_whites);}
					else{filewhites_flag=true;c=orig_upkey(key_whites);}
					setprefs(mask_filewhites,filewhites_flag);
					vis(c,w);
				}else if(chr==key_wrap){if(text_wrap(w)/*true*/)return true;}
				else if(chr==key_swkey){
					quick_pack(com_nr_swkey,change_key)
					if(command((char*)args,(show_key_struct){key_swkey,0})==command_resize)return true;
					wmove(w,getcury(w),getcurx(w));
				}else if(chr==key_tab){
					quick_pack(com_nr_tab,change_tab_size)
					if(command((char*)args,(show_key_struct){key_tab,0})==command_false)wmove(w,getcury(w),getcurx(w));
					else return true;
				}else type(c,w);//enter, tab, ^, unknown ctrls
			}else{
				if(strcmp(s,"KEY_F(1)")==0){
					int cy=getcury(w);int cx=getcurx(w);
					phelp=0;
					int i=helpshow(0);
					int mx=getmaxy(stdscr)-2;
					for(;i<mx;i++){move(i,0);clrtoeol();}
					helpshowlastrow(mx);
					if(helpin(w)/*true*/){
						ungetch(c);
						return true;
					}
					wmove(w,cy,cx);
				}
				//i saw these only when mousemask is ALL_MOUSE_EVENTS : focus in, focus out
				else if(strcmp(s,"kxIN")==0||strcmp(s,"kxOUT")==0){}
				else type(c,w);
				//continue;
			}
		}
	}
}
#define normalize_yes_clue ok=normalize_yes;*normalize_clue_pointer=*r;
//-1 to normalize, 0 errors, 1 ok
static normalize_char normalize(char**c,size_t*size,size_t*r,size_t*normalize_clue_pointer){
	normalize_char ok=normalize_err;
	char*text_w=c[0];
	size_t sz=size[0];
	char*norm=(char*)malloc(2*sz+1); //+1, there are many curses addstr
	if(norm!=nullptr){
		size_t j=0;ok=normalize_ok;
		for(size_t i=0;i<sz;i++){
			char a=text_w[i];
			if(a=='\n'){
				r[0]++;
				if(ln_term_sz==2){
					norm[j]='\r';j++;normalize_yes_clue
				}
				else if(ln_term[0]=='\r'){a='\r';normalize_yes_clue}
			}else if(a=='\r'){
				r[0]++;
				if(ln_term_sz==2){
					if(((i+1)<sz)&&text_w[i+1]=='\n'){
						norm[j]=a;j++;i++;
						a='\n';}
					else{norm[j]=a;j++;a='\n';normalize_yes_clue}
				}
				else if(ln_term[0]=='\n'){
					if(((i+1)<sz)&&text_w[i+1]=='\n')i++;
					a='\n';normalize_yes_clue
				}
			}
			norm[j]=a;j++;
		}
		norm[j]='\0';size[0]=j;
		free(text_w);c[0]=norm;
	}
	return ok;
}
//same as normalize
static normalize_char normalize_split(char**c,size_t*s,size_t*r,char*argfile){
	if(argfile==nullptr||split_grab(c,s)/*true*/){//if at normalize will work also in open cutbufs but will error at explodes there(save cutbuf with explodes)
		return normalize(c,s,r,&normalize_clue);
	}
	return normalize_err;
}
static void rows_init(size_t size){
	char*b=&text_init_b[size];
	row*z=&rows[0];
	z->data=text_init_b;size_t sz;
	char*a=text_init_b;
	for(size_t i=1;i<rows_tot;i++){
		sz=memtrm(a)-a;
		z->sz=sz;z->spc=0;
		a+=sz+ln_term_sz;
		z=&rows[i];
		z->data=a;
	}
	z->sz=b-a;z->spc=0;
	rows_spc=rows_tot;
}
static bool grab_file(char*f,size_t*text_sz){
	bool fake=true;
	int fd=open(f,O_RDONLY);
	if(fd!=-1){
#ifndef MKDIR_1ARG
		if(is_dir(fd)/*true*/){
#else
		if(is_dir(f)/*true*/){
#endif
			putchar('\"');
			size_t n=strlen(f);
			for(size_t i=0;i<n;i++){
				putchar(f[i]);
			}
			//puts(f);
			puts("\" is a directory");
		}
		else{
			size_t size=lseek(fd,0,SEEK_END);
			if(size!=(size_t)-1){
				text_init_b=(char*)malloc(size);
				if(text_init_b!=nullptr){
					lseek(fd,0,SEEK_SET);
					#pragma GCC diagnostic push
					#pragma GCC diagnostic ignored "-Wunused-result"
					read(fd,text_init_b,size);
					#pragma GCC diagnostic pop
					text_sz[0]=size;
					fake=false;
				}
			}
		}
		close(fd);
	}
	return fake;
}
static bool grab_input(size_t*text_sz){
	size_t s=512;size_t d;
	do{
		void*v=realloc(text_init_b,*text_sz+s);
		if(v==nullptr)return true;
		text_init_b=(char*)v;
		char*c=text_init_b+*text_sz;
		d=(size_t)read(STDIN_FILENO,c,s);//zero indicates end of file
		//char*b=fgets(c,s,stdin);
		//if(b==nullptr)break;
		//d=strlen(c);
		*text_sz+=d;
	}while(d==s);
	FILE* tty=freopen("/dev/tty","r",stdin);
	if(tty!=nullptr)
		return false;
	return true;//it was a problem at input, not sure if was here, anyway here is easy to force with sudo chmod 600 /dev/tty
}

static void getkeys(char kp){
	for(unsigned char i=0;i<number_of_keys;i++){
		unsigned char ix;//this is unsigned because is unknown read
		if(i<kp){
			ix=keys_row_frompref[i];
			if(ix>key_last_index)return;
		}else{//this is very safe for when adding new keys and will write 0 at no changes that can be ok on many users pref files and one program
			ix=keys_row_orig[i];
			//keys_row_frompref[i]=ix; is already at define time
		}
		if(keys_frompref[ix].key_location!=nullptr)return;
		unsigned char ix_orig=keys_row_orig[i];
		memcpy(&keys_frompref[ix],&keys_orig[ix_orig],sizeof(key_struct));
	}
	keys=keys_frompref;
	keys_row=keys_row_frompref;
	for(unsigned char i=0;i<number_of_keys;i++){
		char ix=keys_row[i];
		if(keys_row_orig[i]!=ix){
			changekey(ix);
		}
	}
}

static bool valid_ln_term(int argc,char**argv,bool*not_forced){
	if(argc==3){
		char*input_term=argv[2];
		if(strcmp(input_term,"rn")==0){ln_term[0]='\r';ln_term[1]='\n';ln_term[2]='\0';ln_term_sz=2;}
		else if(strcmp(input_term,"r")==0)ln_term[0]='\r';
		else if(strcmp(input_term,"n")==0){}
		else{
			puts("Line termination argument must be: \"rn\", \"r\" or \"n\".");
			return true;
		}
		*not_forced=false;
	}
	return false;
}
//same as normalize
static normalize_char startfile(char*argfile,int argc,char**argv,size_t*text_sz,bool no_file,bool no_input,bool not_forced){
	if(no_file==false)if(grab_file(argfile,text_sz)/*true*/)return normalize_err;
	if(no_input==false){
		if(no_file/*true*/){
			text_init_b=(char*)malloc(1);//1 is for some systems that at malloc(0) are returning null
			if(text_init_b==nullptr)return normalize_err;
			*text_sz=0;
		}
		//else will be appended to existing file
		if(grab_input(text_sz)/*true*/)return normalize_err;
	}

	if(not_forced/*true*/){
		size_t i=*text_sz;
		while(i>0){ //only one \n or \r to detect
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
		return normalize_split(&text_init_b,text_sz,&rows_tot,argfile);
	}
	if(normalize_split(&text_init_b,text_sz,&rows_tot,argfile)!=normalize_err)return normalize_ok;
	return normalize_err;
}
static bool help_init(char*f,size_t szf){
	size_t sz1=sizeof(hel1)-1;
	size_t sz2=sizeof(hel2);
	char*a=(char*)malloc(sz1+szf+sz2);
	if(a!=nullptr){
		helptext=a;
		memcpy(a,hel1,sz1);
		a+=sz1;memcpy(a,f,szf);
		keys_helptext=a+szf;
		memcpy(keys_helptext,hel2,sz2);
		keys_help=keys_helptext+(sz2-help_last_part_size);
		return true;
	}
	return false;
}
static void getfilebuf(char*cutbuf_file){//,size_t off){
	int f=open(cutbuf_file,O_RDONLY);
	if(f==-1)f=open_new(cutbuf_file);
	/*if(f==-1){
		char store=cutbuf_file[off];
		cutbuf_file[off]='.';
		f=open(cutbuf_file+off,O_RDONLY);
		cutbuf_file[off]=store;
	}*/
	if(f!=-1){
		size_t sz=lseek(f,0,SEEK_END);
		if(sz!=(size_t)-1&&sz!=0){
			char*v=(char*)malloc(sz);
			if(v!=nullptr){
				lseek(f,0,SEEK_SET);
				cutbuf_sz=(size_t)read(f,v,sz);
				size_t not_a_clue_at_cutbuf;
				if(normalize(&v,&cutbuf_sz,&cutbuf_r,&not_a_clue_at_cutbuf)!=normalize_err){
					cutbuf=v;cutbuf_spc=cutbuf_sz;
				}else free(v);
			}
		}
		close(f);
	}
}
static void getprefs(){
	int f=open(prefs_file,O_RDONLY);
	if(f!=-1){
		char mask;
		if(read(f,&mask,mask_size)==mask_size){
			if((mask&mask_mouse)!=0)stored_mouse_mask=~0;
			if((mask&mask_indent)!=0)indent_flag=true;
			if((mask&mask_insensitive)!=0)insensitive=true;
			if((mask&mask_ocompiler)!=0)ocompiler_flag=true;
			if((mask&mask_splits)!=0)splits_flag=true;
			if((mask&mask_filewhites)!=0)filewhites_flag=true;
			bar_byte len;
			if(read(f,&len,extlen_size)==extlen_size){
				ocode_extension_new=(char*)malloc(len+1);
				if(ocode_extension_new!=nullptr){
					if(read(f,ocode_extension_new,len)==len){
						ocode_extension=ocode_extension_new;
						ocode_extension[len]='\0';
						if(split_readprefs(f)/*true*/){
							if(read(f,&len,sizeof(char))==sizeof(char)){
								if(len<=number_of_keys){
									if(read(f,keys_row_frompref,len)==len){
										getkeys(len);
										if(read(f,&len,sizeof(char))==sizeof(char)){
											filewhites_extension_new=(char*)malloc(len+1);
											if(read(f,filewhites_extension_new,len)==len){
												filewhites_extension=filewhites_extension_new;
												filewhites_extension[len]='\0';
												tab_protocol tabtest;
												if(read(f,&tabtest,sizeof(tab_protocol))==sizeof(tab_protocol)){
													if(tab_conditions_nr(tabtest)){
														tab_sz=tabtest;
													}
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
		close(f);
	}
	//is ok to have the default prefs always, even if is not popular, some features are good to be activated
	//f=open_new(prefs_file);if(f!=-1){mask=mask_indent;writeprefs(f,mask);}
}
static bool help_cutbuffile_preffile(char*s,char*cutbuf_file){
#if ((!defined(USE_FS)) && (!defined(USE__FS)))
	set_path_separator(s);
#endif
	size_t sz=strlen(s);size_t i=sz;
	do{
		i--;
		char a=s[i];
		if(a==path_separator){i++;break;}
	}while(i!=0);
	size_t exenamesize=sz-i;
	bool b=help_init(&s[i],exenamesize);
	char*h=getenv("HOME");
	if(h!=nullptr){
		size_t h_sz=strlen(h);
		if(h_sz!=0){
			size_t info_sz=h_sz+1+1+exenamesize+4+1; //+separator +dot +info +null
			if(info_sz<=max_path_0){
			#define storeflname "%c.%sinfo"
				sprintf(cutbuf_file,"%s" storeflname,h,path_separator,&s[i]);
				//this must be after line termination is observed: getfilebuf(cutbuf_file);

				const char*conf=".config";
				size_t csz=strlen(conf)+1;//plus separator
				if(info_sz+csz<=max_path_0){
					int dirsz=sprintf(prefs_file,"%s%c%s",h,path_separator,conf);
					if(access(prefs_file,F_OK)==-1){
#ifndef MKDIR_1ARG
						mkdir(prefs_file,0777);
#else
						mkdir(prefs_file);
#endif
						//the mode of the created directory is (mode & ~umask & 0777)
						//0 on success
					}
					sprintf(&prefs_file[dirsz],storeflname,path_separator,&s[i]);
				}
			}
		}
	}
	return b;
}
static void writefilebuf(char*cutbuf_file){
	if(cutbuf_file[0]!=0){
		int f=open(cutbuf_file,O_WRONLY|O_TRUNC);
		if(f!=-1){
			#pragma GCC diagnostic push
			#pragma GCC diagnostic ignored "-Wunused-result"
			write(f,cutbuf,cutbuf_sz);
			#pragma GCC diagnostic pop
			close(f);
		}
	}
}
static void color(){
	if(start_color()!=ERR){
		if(init_pair(color_a,COLOR_BLACK,COLOR_WHITE)!=ERR){//TERM vt100
			if(init_pair(color_b,COLOR_BLACK,COLOR_CYAN)!=ERR){
				if(init_pair(color_c,COLOR_BLACK,COLOR_GREEN)!=ERR){
					init_pair(color_d,COLOR_BLACK,COLOR_YELLOW);
				}
			}
		}
	}
}

static void proced(char*cutbuf_file,WINDOW*w1){
	bool loops;
	int cy=0;int cx=0;
	int r=getmaxy(stdscr)-1;
	int old_r=r-1;//set -1 because at first compare is erasing new_visual
	do{
		void*a=realloc(x_right,(size_t)r);
		if(a==nullptr)break;
		x_right=(bool*)a;//is text,[xtext+nothing
		int maxx=getmaxx(stdscr);
		int c=maxx-(leftspace+contentmarginsize);
		tabs_rsz=1+(c/tab_sz);
		if((c%tab_sz)!=0)tabs_rsz++;
		void*b=realloc(tabs,sizeof(int)*(size_t)(r*tabs_rsz));
		if(b==nullptr)break;
		tabs=(int*)b;//is nroftabs,col0,col1,...; and int if 256 tabs. not short? moving like curses with the ints
		a=realloc(mapsel,(size_t)c+1);
		if(a==nullptr)break;
		mapsel=(char*)a;//cols+null

		if(textfile!=nullptr){
			move(0,0);//no clear, only overwrite, can resize left to right then back right to left
			write_title();//this is also the first write
		}

		loops=false;
		WINDOW*w=newwin(r-topspace,c,topspace,leftspace);//The functions which return a window pointer may also fail if there is insufficient memory for its data structures.
		if(w!=nullptr){
			leftcontent=newwin(r-topspace,contentmarginsize,topspace,0);
			if(leftcontent!=nullptr){
				syntaxcontent=newwin(r-topspace,contentmarginsize,topspace,contentmarginsize);
				if(syntaxcontent!=nullptr){
					rightcontent=newwin(r-topspace,contentmarginsize,topspace,maxx-contentmarginsize);
					if(rightcontent!=nullptr){
						keypad(w,true);

						bar_init();
						if(r<=old_r)clrtoeol();//resize to up,is over text
						//or =, clear bar,visual and saves
						old_r=r;

						if(split_reminder_c>=split_yes_mixless){
							if(split_reminder_c>=split_yes_mix)
								visual_write(splits_activated);
							else
								visual_write(splits_activated_mixless);
							//visual_bool=true;//to clear at a next key
						}
						if(mod_flag==false){
							if(hardtime==0)restore_visual();//has wnoutrefresh(stdscr)
							else mod_visual(modif_visual);//has wnoutrefresh(stdscr)
						}
						else wnoutrefresh(stdscr);

						refreshpage(w);//this must be after refresh stdscr, else first > at rightcontent will not show
						wmove(w,cy,cx);

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
					}
				}
			}
		}
		delwin(w);delwin(leftcontent);delwin(syntaxcontent);delwin(rightcontent);//returns an error if the window pointer is null
	}while(loops/*true*/);//this will reenter only if loopin() said
	if(x_right!=nullptr){
		free(x_right);
		if(tabs!=nullptr){
			free(tabs);
			if(mapsel!=nullptr){
				free(mapsel);//from here it is the interaction
				writefilebuf(cutbuf_file);//only here can be modified than the initial state
				undo_free();//when using undo
			}
		}
	}
}
static void remove_config_print(char*s){
	printf("%s removed\n",s);
}
static bool remove_config(char*pattern,char*cutbuf_file){
	if(strcmp(pattern,"--remove-config")==0){
		int c=-1;int p=-1;int pf=-1;
		char prefs_folder[max_path_0];
		if(cutbuf_file[0]!='\0')c=access(cutbuf_file,F_OK);
		if(prefs_file[0]!='\0'){
			char*end=prefs_file+strlen(prefs_file);
			do{end--;}while(*end!=path_separator);
			*end='\0';sprintf(prefs_folder,"%s",prefs_file);*end=path_separator;
			p=access(prefs_folder,F_OK);//is also working for folders
			if(p==0)pf=access(prefs_file,F_OK);
		}
		if(c==0||p==0){
			puts("Would remove:");//puts writes and a trailing new line
			if(c==0)puts(cutbuf_file);
			if(p==0){
				if(pf==0)puts(prefs_file);
				printf("\"%s\" if is empty\n",prefs_folder);
			}
			puts("yes ?");
			int e=getchar();//can be fgets but at raw was not ok, and initscr is clearing screen and can't see printf on some systems, if can combine raw with initscr this is ok
			if(e=='y'){
				e=getchar();
				if(e=='e'){
					e=getchar();
					if(e=='s'){
					//here can also verify for newline
						if(c==0)
							if(unlink(cutbuf_file)==0)
								remove_config_print(cutbuf_file);
						if(p==0){
							if(pf==0)if(unlink(prefs_file)==0)
								remove_config_print(prefs_file);
							if(rmdir(prefs_folder)==0)remove_config_print(prefs_folder);
							else printf("%s ignored (maybe is not empty)\n",prefs_folder);
						}
						return true;
					}
				}
			}
			puts("expecting \"yes\"");
		}
		return true;
	}
	return false;
}
static bool get_answer(char switcher){
	int c=getchar();
	bool a=c==switcher;
	while(c!='\n')c=getchar();
	return a;
}

static void filewhites_read(){
	for(size_t i=0;i<rows_tot;i++){
		row*r=&rows[i];
		char*d=r->data;
		void*last=d+r->sz;
		while(d!=last){
			if(*d!=' ')break;
			*d='\t';
			d++;
		}
	}
}

static void freeprefs(){//these if are not from start , they can be when user decides to enter new prefs, and an alloc is there
	if(ocode_extension_new!=nullptr)free(ocode_extension_new);//also need it at change for view what is was
	split_freeprefs();
	if(filewhites_extension_new!=nullptr)free(filewhites_extension_new);
}
static void action_go(int argc,char**argv,char*cutbuf_file,char*argfile){
	if(prefs_file[0]!='\0')getprefs();//split is first that depends on prefs
	for(unsigned char i=0;i<number_of_keys;i++){//this or set only at modifications
		unsigned char k=keys_row[i];
		keys_help[k]=k+'A';
	}

	size_t text_sz;
	bool not_forced=true;
	bool no_file=argc==1;
	if(no_file==false){
		char*src=argv[1];
		if(remove_config(src,cutbuf_file)/*true*/)return;
		size_t f_slen=strlen(src);
		argfile=(char*)malloc(f_slen+1);//textfile= is not ok, can be changed at =input. is also set at new visual, and below
		if(argfile==nullptr)return;
		char*dest=argfile;char*end=src+f_slen;
		while(src<end){
			if(*src=='\\'){src++;
				if(src==end)break;
			}
			*dest=*src;dest++;src++;
		}*dest='\0';

		no_file=new_visual(argfile)/*true*/;
		if(restorefile_path(argfile)/*true*/){
			if(access(restorefile_buf,F_OK)==0){
				//if(argc==2){
				puts("There is an unrestored file, (c)ontinue?");
				if(get_answer('c')==false)return;
				//}
			}
		}
		if(editingfile_path(argfile)/*true*/){
			if(access(editingfile_buf,F_OK)==0){
				puts("The file is already opened in another instance, (c)ontinue?");
				if(get_answer('c')==false)return;
			}else editing_new();
		}
		if(valid_ln_term(argc,argv,&not_forced)/*true*/)return;
		//if(no_file/*true*/)
		split_reminder_c=split_conditions(argfile,true);//if or if not the file exists
	}

	//#define POLLIN 0x0001
	//typedef unsigned int nfds_t;
	//struct pollfd{int    fd;short  events;short  revents;};
	//int poll(struct pollfd fds[], nfds_t nfds, int timeout);
	//struct pollfd fds[1];
	////typedef struct __sFILE FILE;
	////FILE* stdin __attribute__((annotate("introduced_in=" "23")));
	//fds[0].fd = STDIN_FILENO;
	//fds[0].events = POLLIN;
	//bool no_input=poll(fds, 1, 0)<1;
	bool no_input=isatty(STDIN_FILENO);

	normalize_char ok=0;
	if(no_file/*true*/&&no_input/*true*/){
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
		ok=startfile(argfile,argc,argv,&text_sz,no_file,no_input,not_forced);
		if(ok!=normalize_err){
			if(ok==normalize_yes){
				//entering \r in printf at %s will return to the start
				const char*a;const char*b;
				if(ln_term[0]=='\r'){
					a="r";
					if(ln_term[1]=='\n')b="\\n";else b="";
				}else{a="n";b="";}
				printf("Normalize line endings to \\%s%s? Y/n\n",a,b);
				if(get_answer('n')/*true*/)ok=normalize_err;
			}
			if(ok!=normalize_err){
				rows=(row*)malloc(rows_tot*sizeof(row));
				if(rows!=nullptr){
					rows_init(text_sz);
					if(argfile!=nullptr)if(filewhites_flag/*true*/)if(is_extension_ok(filewhites_extension,argfile))filewhites_read();
					textfile=argfile;
					text_init_e=text_init_b+text_sz+1;
				}
				else ok=normalize_err;
			}
		}
	}
	if(ok!=normalize_err){
		WINDOW*w1=initscr();
		if(w1!=nullptr){
			if(cutbuf_file[0]!='\0')getfilebuf(cutbuf_file);//this is here,not after cutbuf_file path is set,but after line termination is final

			//if set 1press_and_4,5 will disable right press (for copy menu) anyway
			//on android longpress to select and copy is a gesture and is different from mouse events
			//the only difference with ALL_..EVENTS is that we want to speed up and process all events here (if there is a curses implementation like that)
			//this was default for android, but nowadays on desktop is not a default
			//stored_mouse_mask=mousemask(ALL_MOUSE_EVENTS,nullptr);//for error, export TERM=vt100
			if(stored_mouse_mask_q)stored_mouse_mask=mousemask(ALL_MOUSE_EVENTS,nullptr);//must set it after initscr to work

			if(ocompiler_flag/*true*/){//this is here, in loop can be set if wanted with enable/disable and rescan keys
				aftercall=init_aftercall();
			}

			use_default_colors();//assume_default_colors(-1,-1);//it's ok without this for color pair 0 (when attrset(0))
			raw();//stty,cooked; characters typed are immediately passed through to the user program. interrupt, quit, suspend, and flow control characters are all passed through uninterpreted, instead of generating a signal
			color();
			WINDOW*pw=position_init();
			if(pw!=nullptr){
				keypad(w1,true);//this here and not at start: Normalize... and other text will not be after clearscreen
				noecho();//characters typed are not echoed
				nonl();//no translation,faster
				proced(cutbuf_file,w1);
				delwin(pw);
			}
			endwin();

			if(normalize_clue!=no_clue)printf("normalize clue: " protocol "\n",normalize_clue);
			if(clue!=no_clue)printf("last row where was an error at split write was: " protocol "\n",clue);
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
	freeprefs();
}
static void action(int argc,char**argv){
	char cutbuf_file[max_path_0];
	cutbuf_file[0]='\0';
	if(help_cutbuffile_preffile(argv[0],cutbuf_file)/*true*/){//this is here, is convenient for remove_config
		char*argfile=nullptr;//example when launching with no args
		action_go(argc,argv,cutbuf_file,argfile);
		if(argfile!=nullptr)free(argfile);
		if(editingfile!=nullptr)unlink(editingfile);//this can be before and after text_init_b, also, this can be if no argfile when rebase is on with a save as..

		free(helptext);
		if(cutbuf!=nullptr)free(cutbuf);//this is init at getfilebuf or if not there at writemembuf
	}
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

	if(argc>3){puts("Too many arguments.");return EXIT_FAILURE;}

	action(argc,argv);
	return user_return;
}
