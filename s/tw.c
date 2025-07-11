
#define is_tw_c
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

#include "base.h"

typedef struct{
	size_t ytext;
	row_dword xtext;
}rowwrap;//is used as an array allocated, to combine need to add this at row, memory waste

row*rowswrap;
row*store_rows;
size_t store_rows_tot;
size_t store_aftercall;

void extra_unlock(size_t y,size_t x,WINDOW*w){
	//restore variables
	rows=store_rows;
	rows_tot=store_rows_tot;
	if(ocompiler_flag/*true*/){
		aftercall=store_aftercall;
	}
	ytext=y;
	xtext=x;
	centering3(w,nullptr,nullptr,false);
	free(rowswrap);
}

bool text_wrap(WINDOW*w){
	//calculate rows required
	size_t aftercall_aux;
	int max=getmaxx(w);
	size_t n=0;
	for(size_t i=0;i<rows_tot;i++){
		if(ocompiler_flag/*true*/){
			if(i==aftercall)aftercall_aux=n;
		}
		row*r=&rows[i];
		if(r->sz==0)n++;
		else{
			row_dword sz=r->sz;

			//add tabs
			char*s=r->data;const char*end=s+sz;
			int j=0;for(j=0;s!=end;s++)if(*s=='\t')j++;
			sz+=(tab_sz-1)*j;

			n+=sz/max;

			int rem=sz%max;
			if(rem!=0){//without !=0 is same asm,ok? only for "true" let dependent, else independent
				if(r->data[r->sz-1]!='\t'||rem>=tab_sz)n++;
				//else last char is tab and is on two wrapped rows,do not split the tab on two rows
			}
		}
	}

	rowswrap=(row*)malloc((n*sizeof(row))+(n*sizeof(rowwrap)));//add it after rows, for easy free
	if(rowswrap!=nullptr){
		rowwrap*rowswrap_add=(rowwrap*)(&rowswrap[n]);

		//non-wrapped x,y
		size_t realy;row_dword realx;
		fixed_yx(&realy,&realx,getcury(w),getcurx(w));
		//and to stay in wrapped view
		if(realx==rows[realy].sz&&realx!=0)realx--;

		size_t y;row_dword x;
		size_t j=0;
		for(size_t i=0;i<rows_tot;i++){
			if(i==realy){
				y=j;//the new y pos
			}
			row*r_in=&rows[i];
			size_t k;size_t l=0;size_t m=0;size_t delim=max;
			for(k=0;k<r_in->sz;){
				if(r_in->data[k]=='\t')m+=tab_sz;
				else m++;
				k++;
				if(m>=delim){
					row*r_out=&rowswrap[j];
					r_out->data=&r_in->data[l];
					r_out->sz=k-l;

					//for position
					rowswrap_add[j].ytext=i;
					rowswrap_add[j].xtext=l;

					l=k;delim+=max;j++;
				}
			}
			if(k==0||l!=k){//empty rows,reminder of rows
				row*r_out=&rowswrap[j];
				r_out->data=&r_in->data[l];
				r_out->sz=k-l;

				//for position
				rowswrap_add[j].ytext=i;
				rowswrap_add[j].xtext=l;

				j++;
			}
		}
		//and set new x/y
		x=realx;
		size_t yy=y;
		do{y++;}
		while(y<j&&rowswrap_add[y].ytext==realy&&realx>=rowswrap_add[y].xtext);
		y--;
		if(yy<y)x-=rowswrap_add[y].xtext;//x is on a wrap part

		//visual
		visual(orig_upkey(key_wrap));//without stdscr refresh is not ok

		//tw store some variables
		if(ocompiler_flag/*true*/){
			store_aftercall=aftercall;
			if(aftercall<rows_tot){
				aftercall=aftercall_aux; // if warning, is ok, ocompiler_flag dependent
			}else aftercall=n;
		}
		store_rows=rows;rows=rowswrap;
		store_rows_tot=rows_tot;rows_tot=j;

		//window
		ytext=y;xtext=0;
		size_t r=centeringy(w);
		wmove(w,r,xc_to_c(x,r));

		//loop
		movement_char z;
		do{
			int b=wgetch(w);
			z=movment(b,w);
			if(z==movement_resize){extra_unlock(rowswrap_add[y].ytext,rowswrap_add[y].xtext+x,w);return true;}

			fixed_yx(&y,&x,getcury(w),getcurx(w));
			position_core(rowswrap_add[y].ytext,rowswrap_add[y].xtext+x);
		}while(z!=movement_diffkey);

		visual(' ');
		extra_unlock(rowswrap_add[y].ytext,rowswrap_add[y].xtext+x,w);
	}
	return false;
}
