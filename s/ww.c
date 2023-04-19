
#include "top.h"
//bool,nullptr,...

#ifdef HAVE_STDLIB_H
#include<stdlib.h>
#else
#include"inc/ww/stdlib.h"
#endif
#ifdef HAVE_CURSES_H
#include<curses.h>
#else
#include"inc/curses.h"
#endif

#include "base.h"

static row*rowswrap;
static row*store_rows;
static size_t store_rows_tot;
static size_t store_ytext;

static void ww_free(){
	free(rowswrap);
}
static void ww_unlock(){
	//restore variables
	rows=store_rows;
	rows_tot=store_rows_tot;
	ytext=store_ytext;
}

bool word_wrap(WINDOW*w){
	//calculate rows required
	int max=getmaxx(w);
	size_t n=0;
	for(size_t i=0;i<rows_tot;i++){
		row*r=&rows[i];
		if(r->sz==0)n++;
		else{
			unsigned int sz=r->sz;

			//add tabs
			char*s=r->data;
			int j;for (j=0; s[j]!='\0'; s[j]=='\t' ? j++ : *s++);
			sz+=(tab_sz-1)*j;

			n+=sz/max;

			int rem=sz%max;
			if(rem!=0){//without !=0 is same asm,ok? only for "true" let dependent, else independent
				if(r->data[r->sz-1]!='\t'||rem>=tab_sz)n++;
				//else last char is tab and is on two wrapped rows,do not split the tab on two rows
			}
		}
	}

	rowswrap=(row*)malloc(n*sizeof(row));
	if(rowswrap!=nullptr){
		size_t j=0;
		for(size_t i=0;i<rows_tot;i++){
			if(i==ytext){
				//ww overwrite some variables
				store_ytext=ytext;ytext=j;
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
					//r_out->spc
					l=k;delim+=max;j++;
				}
			}
			if(k==0||l!=k){//and empty rows
				row*r_out=&rowswrap[j];
				r_out->data=&r_in->data[l];
				r_out->sz=k-l;
				j++;
			}
		}

		//ww overwrite some variables
		store_rows=rows;rows=rowswrap;
		store_rows_tot=rows_tot;rows_tot=j;

		//visual
		visual('W');//without stdscr refresh is not ok

		//window
		xtext=0;
		centering_simple(w);

		//loop
		int z;
		do{
			int b=wgetch(w);
			z=movment(b,w);
			if(z==1){ww_unlock();ww_free();return true;}
		}while(z!=0);
		visual(' ');
		ww_unlock();
		centering_simple(w);
	}
	ww_free();
	return false;
}
