#include"main0.h"
//strlen;open,2;close;write,3;free,4
//realloc,3;malloc,4
#include"mainb.h"
//move,20;wmove,5;getch,3;wgetch,3
//getmaxy,10;getmaxx;4;getcury,3
//getcurx,12;stdscr,20;keyname;addch,13
//waddch,2;mvaddch,8;addstr;wnoutrefresh,7
//attrset,6;newwin;COLOR_PAIR,4;strcmp
//sprintf,2
#include"mainbc.h"

#define F_OK 0
/*//#include <sys/stat.h>//arm
struct timespec {
  // Number of seconds.
  long tv_sec;
  // Number of nanoseconds. Must be less
  long tv_nsec;
};
struct stat{
  unsigned long long st_dev;
  unsigned char __pad0[4];
  unsigned long __st_ino;
  unsigned int st_mode;
  unsigned int st_nlink;
  unsigned short st_uid;
  unsigned short st_gid;
  unsigned long long st_rdev;
  unsigned char __pad3[4];
  long long st_size;
  unsigned long st_blksize;
  unsigned long long st_blocks;
  struct timespec st_atim;
  struct timespec st_mtim;
  struct timespec st_ctim;
  unsigned long long st_ino;
};
#define S_IFMT 00170000
#define S_IFDIR 0040000
#define S_ISDIR(m) (((m) & S_IFMT) == S_IFDIR)*/

#ifdef __cplusplus
extern "C"{
#endif

//#include<curses.h>
int mvwaddch(WINDOW*,int,int,const chtype);
int addnstr(const char*,int);//8
int mvaddstr(int,int,const char*);//3
int mvaddnstr(int,int,const char*,int);//3
int mvwaddstr(WINDOW*,int,int,const char*);
int wresize(WINDOW*,int,int);//2
int mvwin(WINDOW*,int,int);//2
int getbegx(const WINDOW*);//3
int getbegy(const WINDOW*);
int wdeleteln(WINDOW*);
//unistd.h
int access(const char*,int);//2              
//stdlib.h
int atoi(const char*);//3
//sys/stat.h
//int fstat(int,struct stat*);
//dirent
void* fdopendir(int);
int closedir(void*);

#ifdef __cplusplus
}
#endif

#define b_inf_s "F1 for help"
#define quest_ex_s "? y/C/n"
static int com_left=sizeof(b_inf_s);
#define max_path 0xff
static char input1[max_path+1];
static char input2[max_path+1];
static char*input=input1;
static WINDOW*poswn;
//static char inputr[max_path+1];
//static size_t cursorr;

typedef struct{
size_t yb;
size_t xb;
size_t ye;
size_t xe;
char*data;}eundo;
static eundo*undos=nullptr;
static size_t undos_tot=0;
static size_t undos_spc=0;
static size_t undos_save=0;
static size_t undos_max=0;
static int undo_v=0;
static bool new_f=false;
#define new_s "New Path"
static bool new_v=false;

const char*bar_init(){
	return b_inf_s;
}
static bool wrt(int f){
	size_t n=rows_tot-1;
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
static bool saving(){
	int f;bool r;
	if(new_f){
		f=open(textfile,O_CREAT|O_WRONLY|O_TRUNC,S_IRUSR|S_IWUSR);
		new_f=f==-1;
	}
	else f=open(textfile,O_WRONLY|O_TRUNC);
	if(f!=-1){
		r=wrt(f);
		close(f);
	}else r=false;
	return r;
}
static void inputpath(){
	textfile=input;
	if(input==input1)input=input2;
	else input=input1;
	//good for other commands that are not storing the result
}
//-1exist,0er,1ok
static int saves(){
	if(access(input,F_OK)==-1){
		inputpath();
		new_f=true;return saving();
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
//1/0/-1 -2
int question(const char*q){
	//if(com_left+sz>getmaxx(stdscr))return 1;
	int y=getmaxy(stdscr)-1;
	mvaddstr(y,com_left,q);
	addstr(quest_ex_s);
	int ch=getch();
	if(ch==KEY_RESIZE)return -2;
	move(y,com_left);
	int sz=(int)(strlen(q)+sizeof(quest_ex_s)-1);
	for(int i=0;i<sz;i++)
		addch(' ');
	//memset(mapsel,' ',sz);mapsel[sz]=0;mvaddstr(y,com_left,mapsel);
	if(ch=='y')return 1;
	else if(ch=='n')return -1;
	return 0;
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
static int inputcmp(char*S1,size_t L1,size_t l2){
	if(l2>L1)return -1;
	size_t n=L1-l2;
	size_t i=0;
	while(i<=n){
		if(S1[i]==input[0]){
			size_t j=1;
			for(;j<l2;j++){
				if(S1[i+j]!=input[j])break;
			}
			if(j==l2)return(int)i;
			i+=j;
		}else i++;
	}
	return -1;
}
static bool findingf(size_t cursor,size_t r,size_t c){
	size_t i=ytext;
	if(i+r>=rows_tot)i=0;
	else{
		i+=r;
		c+=xtext;
		if(c<rows[i].sz){
			int a=inputcmp(rows[i].data+c,rows[i].sz-c,cursor);
			if(a>=0){xtext=c+(size_t)a;ytext=i;return true;}
		}
		i++;
		if(i==rows_tot)i=0;
	}
	size_t b=i;
	size_t e=rows_tot;
	for(;;){
		int a=inputcmp(rows[i].data,rows[i].sz,cursor);
		if(a>=0){xtext=(size_t)a;ytext=i;return true;}
		i++;
		if(i==e){
			if(e==rows_tot){
				if(!b)return false;
				i=0;e=b;
			}
			else return false;
		}
	}
}
static int inputrcmp(char*S1,size_t L1,size_t l2){
	if(l2>L1)return -1;
	size_t m=l2-1;
	size_t i=L1;
	while(l2<=i){
		i--;
		if(S1[i]==input[m]){
			size_t j=m;
			size_t p=i-j;
			while(j>0){
				j--;
				if(S1[p+j]!=input[j]){
					j++;
					break;
				}
			}
			i-=m-j;
			if(!j)return(int)i;
		}
	}
	return -1;
}
static bool findingb(size_t cursor,size_t r,size_t c){
	size_t i=ytext;
	if(i+r>=rows_tot)i=rows_tot-1;
	else{//safe when find first fails
		i+=r;
		c+=xtext;
		if(c<rows[i].sz){
			int n=inputrcmp(rows[i].data,c,cursor);
			if(n>=0){xtext=(size_t)n;ytext=i;return true;}
			if(!i)i=rows_tot-1;
			else i--;
		}
	}
	size_t b=i;
	size_t e=0;
	for(;;){
		int a=inputrcmp(rows[i].data,rows[i].sz,cursor);
		if(a>=0){xtext=(size_t)a;ytext=i;return true;}
		if(i==e){
			if(!e){
				if(b==rows_tot-1)return false;
				i=rows_tot-1;e=b+1;
			}else return false;
		}else i--;
	}
}
static bool finding(size_t cursor,size_t r,size_t c,bool f){
	if(!cursor)return false;
	if(f)return findingf(cursor,r,c);
	return findingb(cursor,r,c);
}
void position(int rw,int cl){
	size_t y=ytext+(size_t)rw;size_t x;
	if(y>=rows_tot){y=rows_tot-1;x=rows[y].sz;}
	else{
		x=xtext+c_to_xc(cl,rw);
		if(x>rows[y].sz)x=rows[y].sz;
	}
	char posbuf[10+1+10+1];
	int n=sprintf(posbuf,"%u,%u",y+1,x+1);
	int dif=getmaxx(poswn)-n;
	if(dif){
		if(dif>0){
			int d=dif;
			wmove(poswn,0,0);
			while(d>0){waddch(poswn,' ');d--;}
			wnoutrefresh(poswn);
		}
		wresize(poswn,1,n);
		mvwin(poswn,getbegy(poswn),getbegx(poswn)+dif);
	}
	mvwaddstr(poswn,0,0,posbuf);
	wnoutrefresh(poswn);
}
static void centering2(WINDOW*w,size_t*rw,size_t*cl,bool right){
	position(0,0);
	int mx=getmaxx(w);
	int wd=mx/3;
	if(right)wd=mx-wd;
	int c=0;char*d=rows[ytext].data;
	size_t xc=xtext;
	do{
		if(!xtext)break;
		xtext--;
		c+=d[xtext]=='\t'?tab_sz:1;
	}while(c<wd);
	size_t hg=(size_t)getmaxy(w)/2;
	if((int)(ytext-hg)<0){hg=ytext;ytext=0;}
	else ytext=ytext-hg;
	refreshpage(w);
	wmove(w,(int)hg,c);
	if(rw){
		rw[0]=hg;cl[0]=xc-xtext;
	}
}
void centering(WINDOW*w,size_t*rw,size_t*cl){
	centering2(w,rw,cl,false);
}
static void colorfind(int a,int y,size_t pos,size_t sz){
	attrset(COLOR_PAIR(a));
	mvaddnstr(y,com_left,input+pos,(int)sz);
	attrset(0);
}
static void colorfindw(int a,int y,size_t pos,size_t sz){
	attrset(COLOR_PAIR(a));
	mvaddnstr(y,com_left,input+pos,(int)sz);
	attrset(0);
	wnoutrefresh(stdscr);
}
/*static bool replace_text(WINDOW*w,int yb){
	vis('R',w);
	wattrset(w,COLOR_PAIR(2));
	cursorr=0;int rstart=yb;
	int rstop=rstart+1;int cstart=getcurx(w);
	for(;;){
		int c=wgetch(w);
		if(c==Char_Return){
			wattrset(w,COLOR_PAIR(0));
			refreshrowsbot(w,rstart,rstop);
			wmove(w,yb,cstart);
			visual(' ');
			return false;
		}
		else if(c==Char_Backspace){
			if(cursorr){
				int x=getcurx(w);int y=getcury(w);
				if(!x){
					x=getmaxx(w)-1;if(y)y--;}
				else x--;
				mvwaddch(w,y,x,' ');
				wmove(w,y,x);
				cursorr--;
			}
		}
		else if(c==KEY_RESIZE)return true;
		else if(cursorr!=max_path&&!no_char((char)c)){
			int cx=getcurx(w);
			if(getmaxx(w)-1==cx){
				int cy=getcury(w);
				if(getmaxy(w)-1==cy){
					wmove(w,0,0);
					wdeleteln(w);
					wmove(w,cy-1,cx);
					rstart=0;
				}else if(cy+1>rstop)rstop++;
			}
			waddch(w,(chtype)c);
			inputr[cursorr]=(char)c;
			cursorr++;
		}
	}
}*/
//1,0,-1resz
static int find(char*z,size_t cursor,size_t pos,size_t visib,int y){
	/*warning: cast from
      'char *' to 'size_t *' (aka
      'unsigned int *') increases required
      alignment from 1 to 4*/
	/*warning: arithmetic on
      a pointer to void is a GNU extension*/
	//z+=sizeof(void*);
	WINDOW*w=((WINDOW**)((void*)z))[1];
	size_t xr=(size_t)getcury(w);
	size_t xc=c_to_xc(getcurx(w),(int)xr);
	//
	size_t sz=cursor-pos;
	if(sz>visib)sz=visib;
	colorfind(1,y,pos,sz);
	//
	if(finding(cursor,xr,xc,true)){
		bool forward=true;
		size_t y1=ytext;size_t x1=xtext;
		bool phase=false;
		wnoutrefresh(stdscr);
		centering(w,&xr,&xc);
		for(;;){
			int a=wgetch(w);
			if(a==Char_Return){
				xc+=cursor;
				forward=true;
			}else if(a==' '){
				forward=false;
			}/*else if(a=='r'){
				if(replace_text(w,getcury(w)))return -1;
			}*/else if(a=='c'){
				return false;
			}else{
				return a==KEY_RESIZE?-1:true;
			}
			finding(cursor,xr,xc,forward);
			if(y1==ytext&&x1==xtext){
				colorfindw(2,y,pos,sz);
				phase=true;
			}else if(phase){
				colorfindw(1,y,pos,sz);
				phase=false;
			}
			centering(w,&xr,&xc);
		}
	}
	int a=getch();
	if(a=='c'){
		return false;
	}
	return a==KEY_RESIZE?-1:true;
}
static bool go_to(int cursor){
	int i=0;size_t y;size_t x;
	for(;;){
		if(input[i]==','){
			input[i]=0;
			y=(size_t)atoi(input);
			x=(size_t)atoi(input+i+1);
			break;
		}
		if(i==cursor){
			y=(size_t)atoi(input);
			x=1;break;
		}
		i++;
	}
	if(y>0&&y<=rows_tot){
		ytext=y-1;
		x--;if(x>rows[ytext].sz)
			xtext=rows[ytext].sz;
		else xtext=x;
		return 1;
	}
	return 0;
}
static void undo_erase(int a){
	int dif=undo_v-a;
	while(dif>0){addch(' ');dif--;}
	undo_v-=undo_v-a;
}
bool bar_clear(){
	if(undo_v){
		move(getmaxy(stdscr)-1,com_left);
		undo_erase(0);
		return true;
	}
	else if(new_v){
		move(getmaxy(stdscr)-1,com_left);
		for(int i=0;(size_t)i<sizeof(new_s)-1;i++)addch(' ');
		new_v=false;
		return true;
	}
	return false;
}
//-2resize,-1no/quit,0er/boolFalse,1ok
int command(char*comnrp){
	int right=getbegx(poswn)-2;
	int rightexcl=right+1;
	int visib=rightexcl-com_left;
	if(visib<2)return 0;//phisical visib is 1
	bar_clear();
	int y=getmaxy(stdscr)-1;
	move(y,com_left);
	int cursor=0;
	int pos=0;int r;
	for(;;){
		int a=getch();
		if(a==Char_Return){
			char comnr=comnrp[0];
			if(!comnr){
				input[cursor]=0;
				r=saves();
				if(r==-1){
					int x=getcurx(stdscr);
					clear_com(y,visib,pos,cursor);
					r=question("Overwrite");
					if(r==1){
						inputpath();
						new_f=false;r=saving();
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
			}else if(comnr==1){
				input[cursor]=0;
				r=go_to(cursor);
			}else{
				int ifback=getcurx(stdscr);
				r=find(comnrp,(size_t)cursor,(size_t)pos,(size_t)visib,y);
				if(r==-1)return -2;
				int dif=rightexcl-getbegx(poswn);
				if(dif!=-1){
					right-=dif+1;rightexcl=right+1;
					visib=rightexcl-com_left;
					if(visib<2)break;
				}
				if(!r){
					//the text was highlighted
					//but can be increased
					//can be resized big,resized small
					//if(dif>=0 here is not relevant
					if(pos)mvaddch(y,com_left-1,'<');
					else move(y,com_left);
					int sz=cursor-pos;bool b=sz>visib;
					if(b)sz=visib;
					addnstr(input+pos,sz);
					if(b)addch('>');
					move(y,ifback>right?right:ifback);
					continue;
				}
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
			if(cursor!=max_path){
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
	}
	clear_com(y,visib,pos,cursor);
	wnoutrefresh(stdscr);
	return r;
}
int save(){
	if(textfile!=nullptr){
		return saving();
	}
	char a=0;
	return command(&a);
}
WINDOW*position_init(){
	poswn=newwin(1,3,0,0);
	return poswn;
}
void position_reset(){
	wresize(poswn,1,3);
	mvwin(poswn,getmaxy(stdscr)-1,getmaxx(stdscr)-5);
}
static bool undo_expand(){
	size_t sz=undos_tot+1;
	size_t dif=sz&row_pad;
	if(dif)sz+=((dif^row_pad)+1);
	if(sz>undos_spc){
		void*v=realloc(undos,sz*sizeof(eundo));
		if(v==nullptr)return true;
		undos=(eundo*)v;undos_spc=sz;
	}
	return false;
}
static void undo_ok(){
	if(undos_tot<undos_save)undos_save=0;
	undos_tot++;undos_max++;
}
static void undo_release(size_t a,size_t b){
	while(a<b){
		if(undos[a].data!=nullptr)
			free(undos[a].data);
		a++;
	}
}
static void undo_newway(){
	if(undos_tot!=undos_max){
		undo_release(undos_tot,undos_max);
		undos_max=undos_tot;
	}
}
bool undo_add(size_t yb,size_t xb,size_t ye,size_t xe){
	if(undo_expand())return true;
	undo_newway();
	eundo*un=&undos[undos_tot];
	un->yb=yb;un->xb=xb;un->ye=ye;un->xe=xe;
	un->data=nullptr;
	undo_ok();return false;
}
static bool undo_del_backward(eundo*un,size_t yb,size_t xb,size_t ye,size_t xe){
	size_t x=sizemembuf(yb,xb,ye,xe);
	char*v=(char*)malloc(x);
	if(v==nullptr)return true;
	un->yb=yb;un->xb=xb;un->ye=ye;un->xe=x;
	un->data=v;
	cpymembuf(yb,xb,ye,xe,v);
	return false;
}
bool undo_add_del(size_t yb,size_t xb,size_t ye,size_t xe){
	if(undo_expand())return true;
	size_t x=sizemembuf(yb,xb,ye,xe);
	size_t dif=x&row_pad;
	if(dif)dif=(dif^row_pad)+1;
	char*v=(char*)malloc(x+dif);
	if(v==nullptr)return true;
	undo_newway();
	eundo*un=&undos[undos_tot];
	un->yb=yb;un->xb=xb;un->ye=ye;un->xe=x;
	un->data=v;
	cpymembuf(yb,xb,ye,xe,v);
	undo_ok();return false;
}
bool undo_add_ind(size_t yb,size_t ye){
	if(undo_expand())return true;
	undo_newway();
	eundo*un=&undos[undos_tot];
	un->ye=yb;un->yb=ye;
	un->data=nullptr;
	undo_ok();return false;
}
static void undo_ind_del(eundo*un,size_t yb,size_t ye,char*d){
	un->yb=ye;un->ye=yb;
	un->data=d;
	for(size_t i=yb;i<ye;i++){
		if(!rows[i].sz)d[i-yb]=ln_term[0];
		else d[i-yb]=rows[i].data[0];
	}
}
bool undo_add_ind_del(size_t yb,size_t ye){
	if(undo_expand())return true;
	void*d=malloc(ye-yb);
	if(d==nullptr)return true;
	undo_newway();
	undo_ind_del(&undos[undos_tot],yb,ye,(char*)d);
	undo_ok();return false;
}
void undo_free(){
	if(undos!=nullptr){
		undo_release(0,undos_max);
		free(undos);
	}
}
static bool dos(WINDOW*w,eundo*un,size_t z){
	char*d=un->data;
	size_t y1=un->yb;size_t y2=un->ye;
	if(y1<=y2){
		size_t xb=un->xb;size_t xe=un->xe;
		if(d!=nullptr){
			if(!paste(y1,xb,&xe,d,xe,y2-y1+1,false))return false;
			un->xe=xe;un->data=nullptr;
			ytext=y2;xtext=xe;
			centering2(w,nullptr,nullptr,true);
			free(d);
		}
		else{
			if(deleting_init(y1,xb,y2,xe))return false;
			if(undo_del_backward(un,y1,xb,y2,xe))return false;
			deleting(y1,xb,y2,xe);
			ytext=y1;xtext=xb;
			centering(w,nullptr,nullptr);
		}
	}else{
		if(d!=nullptr){
			for(size_t i=y2;i<y1;i++){
				row*r=&rows[i];
				if(row_alloc(r,0,1,r->sz))return false;
			}
			un->data=nullptr;
			for(size_t i=y2;i<y1;i++){
				char a=d[i-y2];
				if(a==ln_term[0])continue;
				row*r=&rows[i];
				row_set(r,0,1,r->sz,&a);
			}
			free(d);
			ytext=y1;xtext=1;
			centering2(w,nullptr,nullptr,true);
	}else{
			void*mem=malloc(y1-y2);
			if(mem==nullptr)return false;
			undo_ind_del(un,y2,y1,(char*)mem);
			for(size_t i=y2;i<y1;i++){
				size_t n=rows[i].sz;char*dt=rows[i].data;
				for(size_t j=1;j<=n;j++)dt[j-1]=dt[j];
				rows[i].sz--;
			}
			ytext=y2;xtext=0;
			centering2(w,nullptr,nullptr,false);
		}
	}
	undos_tot+=z;
	if(undos_tot==undos_save)mod_set(true);
	else if(undos_tot==undos_save+z)mod_set(false);
	return true;
}
static void undo_show(size_t n){
	char nr[11];
	int a=sprintf(nr,"%u",n);
	mvaddstr(getmaxy(stdscr)-1,com_left,nr);
	undo_erase(a);
	wnoutrefresh(stdscr);
}
void undo(WINDOW*w){
	if(!undos_tot)return;
	if(dos(w,&undos[undos_tot-1],(size_t)-1)){
		if(undos_tot<=undos_save)undo_show(undos_tot);
		else undo_show(undos_tot-undos_save);
	}
}
void redo(WINDOW*w){
	if(undos_tot==undos_max)return;
	if(dos(w,&undos[undos_tot],1)){
		if(undos_tot>=undos_save)undo_show(undos_max-undos_tot);
		else undo_show(undos_save-undos_tot);
	}
}
void undo_save(){undos_save=undos_tot;}
bool undo_type(size_t yb,size_t xb,size_t ye,size_t xe){
	if(undos_tot){
		eundo*un=&undos[undos_tot-1];
		if(un->data==nullptr&&un->yb<=un->ye){
			if(un->ye==yb&&un->xe==xb){
				un->xe++;return false;
			}
		}
	}
	return undo_add(yb,xb,ye,xe);
}
bool undo_bcsp(size_t yb,size_t xb,size_t ye,size_t xe){
	if(undos_tot){
		eundo*un=&undos[undos_tot-1];
		if(un->data!=nullptr&&un->yb<=un->ye){
			if(un->yb==ye&&un->xb==xe){
				char*d;if(!(un->xe&row_pad)){
					d=(char*)realloc(un->data,un->xe+row_pad+1);
					if(d==nullptr)return true;
					un->data=d;
				}else d=un->data;
				for(size_t i=un->xe;i>0;i--)d[i]=d[i-1];
				d[0]=rows[yb].data[xb];
				un->xb--;un->xe++;
				return false;
			}
		}
	}
	return undo_add_del(yb,xb,ye,xe);
}
bool undo_delk(size_t yb,size_t xb,size_t ye,size_t xe){
	if(undos_tot){
		eundo*un=&undos[undos_tot-1];
		if(un->data!=nullptr&&un->yb<=un->ye){
			if(un->yb==yb&&un->xb==xb){
				char*d;if(!(un->xe&row_pad)){
					d=(char*)realloc(un->data,un->xe+row_pad+1);
					if(d==nullptr)return true;
					un->data=d;
				}else d=un->data;
				d[un->xe]=rows[yb].data[xb];
				un->xe++;
				return false;
			}
		}
	}
	return undo_add_del(yb,xb,ye,xe);
}
void undo_loop(WINDOW*w){
	for(;;){
		int c=wgetch(w);
		if(c==KEY_LEFT)undo(w);
		else if(c==KEY_RIGHT)redo(w);
		else break;
	}
}
bool new_visual(char*f){
	if(access(f,F_OK)==-1){
		new_v=true;new_f=true;textfile=f;
		mvaddstr(getmaxy(stdscr)-1,com_left,new_s);
		return true;
	}
	return false;
}
bool is_dir(int fd){
	//struct stat st;
	//if(fstat(fd,&st)==0)return S_ISDIR(st.st_mode);
	void*d=fdopendir(fd);
	if(d!=nullptr){closedir(d);return true;}
	return false;
}
