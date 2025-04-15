
//here because split.h is not inlcuding curses.h for WINDOW and base.h has that

//main
//bar,tw,tit,split
extern row*rows;
extern size_t rows_tot;
//bar,split
extern char ln_term[3];
extern size_t ln_term_sz;
//split
extern size_t clue;

//bar
//split
bool is_extension_ok(char*,char*);
int open_or_new(char*);
swrite_char wwrite(int,char*,row_dword,swrite_char(*)(int,void*,row_dword));

//sep
//main,split
extern char path_separator;
