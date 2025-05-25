
typedef unsigned long int __ino_t;
typedef unsigned long int __dev_t;
typedef unsigned int __mode_t;
typedef unsigned int __nlink_t;
typedef unsigned int __uid_t;
typedef unsigned int __gid_t;
typedef long int __off_t;
typedef int __blksize_t;
typedef long int __blkcnt_t;
typedef long int __time_t;
typedef long int __syscall_slong_t;
struct timespec
{
    __time_t tv_sec;
    __syscall_slong_t tv_nsec;
};
struct stat
{
	__dev_t st_dev;
	__ino_t st_ino;
	__mode_t st_mode;
	__nlink_t st_nlink;
	__uid_t st_uid;
	__gid_t st_gid;
	__dev_t st_rdev;
	__dev_t __pad1;
	__off_t st_size;
	__blksize_t st_blksize;
	int __pad2;
	__blkcnt_t st_blocks;

	struct timespec st_atim;
	struct timespec st_mtim;
	struct timespec st_ctim;
	int __glibc_reserved[2];
};

#ifdef __cplusplus
extern "C" {
#endif

int mkdir(const char*, __mode_t);
int stat(const char *restrict pathname, struct stat *restrict statbuf);

#ifdef __cplusplus
}
#endif
