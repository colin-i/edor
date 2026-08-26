
//here because split.h is not inlcuding curses.h for WINDOW and base.h has that

//main
//bar,tw,tit,split
extern row*rows;
extern size_t rows_tot;
//bar,split
#define bar_byte unsigned char
extern char ln_term[3];
extern size_t ln_term_sz;
//split
extern size_t clue;

//bar
//main,split
bool is_extension_ok_full(char*,char*,bool);
#define is_extension_ok(a,b) is_extension_ok_full(a,b,true)
//split
#define is_extension_ok_strict(a,b) is_extension_ok_full(a,b,false)
int open_or_new(char*);
swrite_char wwrite(int,char*,row_dword,swrite_char(*)(int,void*,row_dword));

//sep
//main,split
extern char path_separator;

//split
//bar
#define swwrite(a,b,c) wwrite(a,b,c,swrite)
