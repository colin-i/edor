
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

static bool titcolor(char b,char*color,WINDOW*w,bool singlechar){
	singlechar=bar_char(b,w,singlechar);
	if(*color==color_0)*color=color_a;else *color=color_0;
	attrset(COLOR_PAIR(*color));
	return singlechar;
}
static size_t*yvals;
static void position_translated(WINDOW*w){
	size_t y;row_dword x;
	fixed_yx(&y,&x,getcury(w),getcurx(w));
	position_core(yvals[y],x);
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

	size_t nn=n==0?1:n;//to fast position_core, and is good to not malloc(0)
	rowswrap=(row*)malloc((nn*sizeof(row))+(nn*sizeof(size_t)));//same hack as tw.c, extra alloc to free once
	if(rowswrap!=nullptr){
		store_rows=rows;
		store_rows_tot=rows_tot;
		if(ocompiler_flag/*true*/){
			store_aftercall=aftercall;
		}

		//only if wanting to stay where it is now, but we are not searching from start at the moment, so it is a 50/50 case
		//size_t ytext_dif=~0;size_t near_ytext=0;size_t near_ytext_translated=0;
		size_t m=0;yvals=(size_t*)&rowswrap[nn];
		size_t orig_ytext;row_dword orig_xtext;
		fixed_yx(&orig_ytext,&orig_xtext,getcury(w),getcurx(w));

		if(n!=0){
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
							//size_t dif=i>ytext?i-ytext:ytext-i;
							//if(dif<ytext_dif){
							//	dif=ytext_dif;near_ytext=i;near_ytext_translated=m;
							//}
							m++;
						}
					}
				}
			}
			rows_tot=n;
		}else{
			row*rw=&rowswrap[0];
			rw->sz=0;
			*yvals=0;
			rows_tot=1;
		}

		rows=rowswrap;
		ytext=0;//ytext=near_ytext_translated;
		xtext=0;

		//visual
		bar_clear();
		visual('H');
		refreshpage(w);//visual shows now only with this
		wmove(w,0,0);  //                        or this
		position_translated(w);//position_core(near_ytext,0);

		//loop
		char color=color_0;
		movement_char z;bool singlechar=true;
		do{
			int b=wgetch(w);
			z=movment(b,w);
			if(z==movement_resize){extra_unlock(orig_ytext,orig_xtext,w);return true;}
			else if(z==movement_processed){
				position_translated(w);
				continue;
			}
			int r=getcury(w);
			size_t y=ytext+r;
			if(b==Char_Return){
				if(y<rows_tot){
					orig_ytext=yvals[y];
					orig_xtext=0;//or fixed_x(y,&orig_xtext,r,getcurx(w)) but is not important and is a 50/50 case
				}
				break;
			}
			const char*kname=keyname(b);
			if(*kname==Char_Ctrl&&kname[1]=='Q')break;

			//find next row start same as [0,x)+b and wmove there
			singlechar=titcolor(b,&color,w,singlechar);
			if(y<rows_tot){
				row*rw=&rows[y];
				char*data=rw->data;
				row_dword sz;
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
								position_core(yvals[y],sz);
								break;
							}
						}
					}
					y++;
				}
			}
		}while(true);

		if(color!=color_0)attrset(color_0);//reset back
		bar_char(' ',w,singlechar);//and clear, can set new_v or err for later clear but is extra
		visual(' ');
		extra_unlock(orig_ytext,orig_xtext,w);//here xtext to be what was Enter selected now
		position_core(orig_ytext,orig_xtext);
	}

	return false;
}
