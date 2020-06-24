
typedef struct DIR DIR;

#ifdef __cplusplus
extern "C" {
#endif

DIR* fdopendir(int);
int closedir(DIR*);

#ifdef __cplusplus
}
#endif
