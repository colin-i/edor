#include"main0.h"
//strlen,2;open,2;close;write,3
#include"mainb.h"
//move,17;wmove;getch,3;wgetch;getmaxy,3
//getmaxx;2;getcury;getcurx,8;stdscr,14
//keyname;strcmp;addch,10;mvaddch,7
//addstr;wnoutrefresh,5;attrset,2
//COLOR_PAIR
#include"mainbc.h"

//#include<curses.h>
int addnstr(const char*,int);//7
int mvaddstr(int,int,const char*);
int mvaddnstr(int,int,const char*,int);//3
//unistd.h
#define F_OK 0
int access(const char*,int);              
//stdlib.h
int atoi(const char*);

static int com_left;
#define max_path 0xff
//10
static char input1[max_path+1];
static char input2[max_path+1];
static char*input=input1;

char*bar_init(){
	char*h="F1 for help";
	com_left=(int)(strlen(h)+1);
	return h;
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
static bool saving(bool creat){
	int f;bool r;
	if(creat)f=open(textfile,O_CREAT|O_WRONLY|O_TRUNC,S_IRUSR|S_IWUSR);
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
		return saving(true);
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
int question(char*q){
	//if(com_left+sz>getmaxx(stdscr))return 1;
	int y=getmaxy(stdscr)-1;
	mvaddstr(y,com_left,q);
	addstr("? y/C/n");
	int ch=getch();
	if(ch==KEY_RESIZE)return -2;
	move(y,com_left);
	int sz=(int)(strlen(q)+sizeof("? y/C/n"));
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
void centering(WINDOW*w,size_t*rw,size_t*cl){
	size_t wd=(size_t)getmaxx(w)/3;
	size_t c=0;char*d=rows[ytext].data;
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
	wmove(w,(int)hg,(int)c);
	if(rw){
		rw[0]=hg;cl[0]=xc-xtext;
	}
}
static void colorfind(int a,int y,int pos,int sz){
	attrset(COLOR_PAIR(a));
	mvaddnstr(y,com_left,input+pos,sz);
	attrset(0);
}
static bool find(char*z,int cursor,int pos,int visib,int y){
	/*warning: cast from
      'char *' to 'size_t *' (aka
      'unsigned int *') increases required
      alignment from 1 to 4*/
	/*warning: arithmetic on
      a pointer to void is a GNU extension*/
	//z+=sizeof(void*);
	WINDOW*w=((WINDOW**)((void*)z))[1];
	size_t rw=(size_t)getcury(w);
	size_t cl=c_to_xc(getcurx(w),(int)rw);
	//
	int sz=cursor-pos;
	if(sz>visib)sz=visib;
	int ifback=getcurx(stdscr);
	colorfind(1,y,pos,sz);
	//
	bool forward=true;
	size_t y1;size_t x1;int phase=-1;
	for(;;){
		bool b=finding((size_t)cursor,rw,cl,forward);
		if(b){
			if(phase==-1){
				y1=ytext;x1=xtext;
				phase=0;
				wnoutrefresh(stdscr);
			}else if(y1==ytext&&x1==xtext){
				colorfind(2,y,pos,sz);
				phase=1;
				wnoutrefresh(stdscr);
			}else if(phase==1){
				phase=0;
				colorfind(1,y,pos,sz);
				wnoutrefresh(stdscr);
			}
			centering(w,&rw,&cl);
		}
		//else wmove(w,getcury(w),getcurx(w));
		int a;
		if(b)a=wgetch(w);
		else a=getch();
		if(a=='n'){
			if(b)cl++;
			forward=true;
			continue;
		}
		if(a=='p'){
			forward=false;continue;
		}
		if(a=='c'){
			//if(sz){
			mvaddnstr(y,com_left,input+pos,sz);
			move(y,ifback);
			//}else move(y,getcurx(stdscr));//wnoutrefresh(stdscr)
			return false;
		}
		return true;
	}
}
//-2resize,-1no/quit,0er,1okSave,...
int command(char*comnrp){
	int right=getmaxx(stdscr)-4;//25
	int rightexcl=right+1;
	int visib=rightexcl-com_left;
	if(visib<2)return 0;//phisical visib is 1
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
						r=saving(false);
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
				r=atoi(input);
			}else{
				r=find(comnrp,cursor,pos,visib,y);
				if(!r)continue;
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
	clear_com(y,visib,pos,cursor);
	wnoutrefresh(stdscr);
	return r;
}
int save(){
	if(textfile){
		return saving(false);
	}
	char a=0;
	return command(&a);
}
