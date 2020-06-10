#include"main0.h"
//strlen,3;open;close;write,2;free,2
//realloc;malloc
#include"mainc.h"
#include"mainbc.h"

typedef struct{
	char*name;
	char*a;
	size_t n;
	char b;
}inst;
static inst*insts;
static size_t insts_sz;
static size_t insts_spc;

static bool out_chr(int f,char c){
	ssize_t s=write(f,&c,1);
	if(s!=1)return true;
	return false;
}
static bool out_wrt(int f,char*c){
	size_t n=strlen(c);
	size_t s=(size_t)write(f,c,n);
	if(s!=n)return true;
	return false;
}
static bool out_wr(int f){
	if(out_wrt(f,".global main\n"))return false;
	if(out_wrt(f,".extern exit\n"))return false;
	if(out_wrt(f,"main:\n"))return false;
	for(size_t i=0;i<insts_sz;i++){
		inst*a=&insts[i];
		if(out_wrt(f,a->name))return false;
		if(out_chr(f,' '))return false;
		if(out_wrt(f,a->a))return false;
		for(size_t j=0;j<a->n;j++){
			if(out_wrt(f,",#"))return false;
			if(out_chr(f,a->b+0x30))return false;
		}
		if(out_chr(f,'\n'))return false;
	}
	return true;
}
static bool insts_expand(size_t n){
	size_t size=insts_sz+n;
	if(size>insts_spc){
		size_t dif=size&0xf;
		size_t sz=size;
		if(dif)sz+=(dif^0xf)+1;
		inst*d=(inst*)realloc(insts,sz*sizeof(inst));
		if(d==nullptr)return false;
		insts=d;insts_spc=sz;
	}
	insts_sz=size;
	return true;
}
static bool out_parse(row*rws,size_t sz){
	if(sz>1||rws[0].sz!=1||rws[0].data[0]<0x30||rws[0].data[0]>0x39)return false;
	inst*i=&insts[0];i->name="mov";i->a="r0";
	i->n=1;
	i->b=rws[0].data[0]-0x30;
	i=&insts[1];i->name="b";i->a="exit";
	i->n=0;
	return true;
}
bool out_f(char*textfile,row*rws,size_t sz){
	bool r=false;
	size_t n=strlen(textfile);
	char*ext=".s";
	size_t e=strlen(ext);
	size_t p=n+e+1;
	char*s=(char*)malloc(p);
	if(s!=nullptr){
		p--;
		s[p]=0;
		while(e){
			e--;p--;
			s[p]=ext[e];
		}
		while(p){
			p--;
			s[p]=textfile[p];
		}
		int f=open(s,O_WRONLY|O_CREAT|O_TRUNC,S_IRUSR|S_IWUSR);
		if(f!=-1){
			insts=nullptr;insts_spc=0;insts_sz=0;
			if(insts_expand(2)){
				if(out_parse(rws,sz))
					r=out_wr(f);
				free(insts);
			}
			close(f);
		}
		free(s);
	}
	return r;
}
