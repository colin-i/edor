
#include "top.h"
//bool

#ifdef HAVE_CURSES_H
#include<curses.h>
#else
#include"inc/curses.h"
#endif

#include "base.h"

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
			int j;for (j=0; s[j]; s[j]=='\t' ? j++ : *s++);
			sz+=(tab_sz-1)*j;

			n+=sz/max;
			if((sz%max)!=0){//still true/false question?
				n++;
			}
		}
	}

	//visual and loop
	visual('W');//without stdscr refresh is not ok
	int z;
	do{
		int b=wgetch(w);
		z=movment(b,w);
		if(z==1)return true;
	}while(z!=0);
	visual(' ');
	return false;
}
