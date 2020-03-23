
#include <stdio.h>
//strlen 3 fopen fclose fputc fwrite

typedef enum
{
    TRUE=1, FALSE=0
}bool;
struct inst{
	char str[2];
	char*t;
};
char terms[][2]={{"lr"}};
struct inst instrs[]={
	{"bx",terms[0]},{0}
};
bool out_chr(FILE*f,char c){
        if(fputc(c,f)!=c)return TRUE;
	return FALSE;
}
bool out_wrt(FILE*f,char*c){
	int n=strlen(c);
	size_t s=fwrite(c,1,n,f);
	if(s!=n)return TRUE;
	return FALSE;
}
void out_wr(FILE*f){
	if(out_wrt(f,".global main\n"))return;
	if(out_wrt(f,"main:\n"))return;
	int i=0;
	do{
		struct inst*a=&instrs[i];
		if(a->str[0]==0)return;
		if(out_wrt(f,a->str))return;
		if(out_chr(f,' '))return;
		if(out_wrt(f,a->t))return;
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
		FILE*f=fopen(out,"wb");
		if(f){
			out_wr(f);
			fclose(f);
		}
	}
}
