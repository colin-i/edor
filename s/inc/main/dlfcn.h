typedef struct {
  const char* dli_fname;
  void* dli_fbase;
  const char* dli_sname;
  void* dli_saddr;
} Dl_info;
#ifdef __cplusplus
extern "C" {
#endif
int dladdr(void* __addr, Dl_info* __info);
#ifdef __cplusplus
}
#endif
