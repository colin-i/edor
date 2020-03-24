//#include <string.h>
typedef unsigned int size_t;
extern size_t strlen(const char*);//3
//#include <fcntl.h>
//sys/types.h
typedef unsigned short mode_t;
extern int open(const char*,int,mode_t);
//asm-generic/fcntl.h
#define O_WRONLY 00000001
#define O_CREAT 00000100
#define O_TRUNC 00001000
//linux/stat.h
#define S_IRUSR 00400
#define S_IWUSR 00200
//#include <unistd.h>
extern int close(int);
typedef int ssize_t;
extern ssize_t write(int,const void*,size_t);

typedef enum{TRUE=1,FALSE=0}bool;
struct inst{
	char*str;
	char**t;
};
char terms[][3]={"r0"};
char*_r0_0[]={terms[0],"#0",0};char*_ex[]={"exit",0};
struct inst instrs[]={
	{"mov",_r0_0},{"b",_ex},0
};
bool out_chr(int f,char c){
	ssize_t s=write(f,&c,1);
	if(s!=1)return TRUE;
	return FALSE;
}
bool out_wrt(int f,char*c){
	int n=strlen(c);
	ssize_t s=write(f,c,n);
	if(s!=n)return TRUE;
	return FALSE;
}
void out_wr(int f){
	if(out_wrt(f,".global main\n"))return;
	if(out_wrt(f,".extern exit\n"))return;
	if(out_wrt(f,"main:\n"))return;
	int i=0;
	do{
		struct inst*a=&instrs[i];
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
		}while(TRUE);
		if(out_chr(f,'\n'))return;
		i++;
	}while(TRUE);
}
int main(int argc,char**argv){
	if(argc==2){
#define mx_pt 100
		int m=mx_pt;
		char*nm=argv[1];
		int s=strlen(nm);
		if(m==s)m--;
		char*ext=".s";
		int e=strlen(ext);
		m-=e;
		int n=m<s?m:s;
		char out[mx_pt+1];
		int i=0;
		for(;i<n;i++){
			out[i]=nm[i];
		}
		for(int j=0;j<e;j++){
			out[i]=ext[j];i++;
		}
		out[i]=0;
		struct inst instr[1];
		instr[0]=instrs[0];
		int f=open(out,O_WRONLY|O_CREAT|O_TRUNC,S_IRUSR|S_IWUSR);
		if(f!=-1){
			out_wr(f);
			close(f);
		}
	}
}
