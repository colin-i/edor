
typedef struct{
unsigned long long d_ino;//uint64_t
//... not used in this program
//only for empty struct warning
} DIR;

#ifdef __cplusplus
extern "C" {
#endif

DIR* fdopendir(int);
int closedir(DIR*);

#ifdef __cplusplus
}
#endif
