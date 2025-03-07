
#include "top.h"
//bool,nullptr,...

#ifdef HAVE_STDLIB_H
#include<stdlib.h>
#else
#include"inc/tit/stdlib.h"
#endif
#ifdef HAVE_CURSES_H
#include<curses.h>
#else
#include"inc/curses.h"
#endif

#include "base.h"

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
		visual('H');
		refreshpage(w);//visual shows now only with this
		wmove(w,0,0);  //                        or this

		//loop
		int z;
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
			/*if(y<rows_tot){
				row*rw=rows[y];
				char*data=rw->data;
				size_t x=fixed_x(y,&x,r,getcurx(w));
				y++;
				while(y<rows_tot){
					row*rw2=rows[y];
					size_t sz2=rw2->sz;
					if(sz2>sz){//only if +b
						if(memcmp(data,rw2->data,sz)==0){
							if(rw2->data[sz]==b){
								ytext=y;
								centeringy(w);
							}
						}
					}
					y++;
				}
			}*/
		}while(true);

		visual(' ');
		extra_unlock(orig_ytext,orig_xtext,w);
	}

	return false;
}
