
#define is_tit_c
#ifdef HAVE_STDDEF_H
#include<stddef.h>
#else
#include"inc/stddef.h"
#endif

#include "top.h"
//bool,nullptr,...

#ifdef HAVE_CURSES_H
#include<curses.h>
#else
#include"inc/curses.h"
#endif
#ifdef HAVE_STDLIB_H
#include<stdlib.h>
#else
#include"inc/stdlib.h"
#endif
#ifdef HAVE_STRING_H
#include<string.h>
#else
#include"inc/string.h"
#endif

#include "base.h"

static void titcolor(char b,char*color,WINDOW*w){
	bar_char(b,w);
	if(*color==color_0)*color=color_a;else *color=color_0;
	attrset(COLOR_PAIR(*color));
}

bool titles(WINDOW*w){
	//calculate rows required
	size_t n=0;
	for(size_t i=0;i<rows_tot;i++){
		row*r=&rows[i];
		if(r->sz>1){
			char*s=r->data;
			if(*s!='\t'&&*s!=' '){
				s+=r->sz-1;
				if(*s=='{'||*s==')'){
					n++; //func ... { //func ... )
				}
			}
		}
	}

	rowswrap=(row*)malloc((n*sizeof(row))+(n*sizeof(size_t)));//same hack as tw.c, extra alloc to free once
	if(rowswrap!=nullptr){
		store_rows=rows;
		store_rows_tot=rows_tot;
		if(ocompiler_flag/*true*/){
			store_aftercall=aftercall;
		}

		size_t m=0;size_t*yvals=(size_t*)&rowswrap[n];
		for(size_t i=0;m!=n;i++){
			row*r=&rows[i];
			if(r->sz>1){
				char*s=r->data;
				if(*s!='\t'&&*s!=' '){
					s+=r->sz-1;
					if(*s=='{'||*s==')'){
						row*rw=&rowswrap[m];
						rw->data=r->data;
						rw->sz=r->sz;
						if(i>=aftercall)aftercall=m;// else let to be old rows_tot value? it looks ok knowing is comparing with a bigger rows_tot
						yvals[m]=i;
						m++;
					}
				}
			}
		}

		size_t orig_ytext;size_t orig_xtext;
		fixed_yx(&orig_ytext,&orig_xtext,getcury(w),getcurx(w));

		rows=rowswrap;rows_tot=n;
		ytext=0;xtext=0;

		//visual
		bar_clear();
		visual('H');
		refreshpage(w);//visual shows now only with this
		wmove(w,0,0);  //                        or this

		//loop
		char color=color_0;
		char z;
		do{
			int b=wgetch(w);
			z=movment(b,w);
			if(z==1){extra_unlock(orig_ytext,orig_xtext,w);return true;}
			else if(z==-1)continue;
			int r=getcury(w);
			size_t y=ytext+r;
			if(b==Char_Return){
				if(y<rows_tot){
					orig_ytext=yvals[y];
				}
				break;
			}
			//find next row start same as [0,x)+b and wmove there
			titcolor(b,&color,w);
			if(y<rows_tot){
				row*rw=&rows[y];
				char*data=rw->data;
				size_t sz;
				int c=getcurx(w);
				fixed_x(y,&sz,r,c);
				y++;
				while(y<rows_tot){
					row*rw2=&rows[y];
					if(rw2->sz>sz){//only if +b
						if(memcmp(data,rw2->data,sz)==0){
							if(rw2->data[sz]==b){
								ytext=y;
								wmove(w,centeringy(w),c);
								break;
							}
						}
					}
					y++;
				}
			}
		}while(true);

		if(color!=color_0)attrset(color_0);//reset back
		bar_char(' ',w);//and clear, can set new_v or err for later clear but is extra
		visual(' ');
		extra_unlock(orig_ytext,orig_xtext,w);
	}

	return false;
}
