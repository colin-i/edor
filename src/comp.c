#include"main.h"
//strlen,3;open;close;write,2;malloc;free

//asm-generic/fcntl.h
#define O_CREAT 00000100
//linux/stat.h
#define S_IRUSR 00400
#define S_IWUSR 00200

typedef struct{
	char*str;
	char**t;
}inst;
static char terms[][3]={"r0"};
static char*_r0_0[]={terms[0],"#0",0};
static char*_ex[]={"exit",0};
static inst instrs[]={
	{"mov",_r0_0},{"b",_ex},{0}
};

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
static void out_wr(int f){
	if(out_wrt(f,".global main\n"))return;
	if(out_wrt(f,".extern exit\n"))return;
	if(out_wrt(f,"main:\n"))return;
	int i=0;
	do{
		inst*a=&instrs[i];
		if(a->str==0)return;
		if(out_wrt(f,a->str))return;
		if(out_chr(f,' '))return;
		if(out_wrt(f,a->t[0]))return;
		int j=1;
		do{
			char*b=a->t[j];
			if(b==0)break;
			if(out_chr(f,','))return;
			if(out_wrt(f,b))return;
			j++;
		}while(true);
		if(out_chr(f,'\n'))return;
		i++;
	}while(true);
}
extern void out_f(char*textfile){
	size_t n=strlen(textfile);
	char*ext=".s";
	size_t e=strlen(ext);
	size_t p=n+e+1;
	char*s=(char*)malloc(p);
	if(s){
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
			out_wr(f);
			close(f);
		}
		free(s);
	}
}
