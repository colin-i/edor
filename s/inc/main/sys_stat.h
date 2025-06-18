
#undef platform_is_known
#ifdef SIZEOF_INT_P
#	if SIZEOF_INT_P >= 8
#		ifdef SIZEOF_LONG
#			define platform_is_known 1
#			if SIZEOF_LONG == 4

typedef unsigned int _dev_t;
typedef unsigned short _ino_t;
typedef long _off_t;
struct stat {
    _dev_t st_dev;
    _ino_t st_ino;
    unsigned short st_mode;
    short st_nlink;
    short st_uid;
    short st_gid;
    _dev_t st_rdev;
    _off_t st_size;
    time_t st_atime;
    time_t st_mtime;
    time_t st_ctime;
};
typedef unsigned short __mode_t;

#			else

typedef unsigned long int __ino_t;
typedef unsigned long int __dev_t;
typedef unsigned int __mode_t;
typedef unsigned int __nlink_t;
typedef unsigned int __uid_t;
typedef unsigned int __gid_t;
typedef long int __off_t;
typedef int __blksize_t;
typedef long int __blkcnt_t;
typedef time_t __time_t;
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

#			endif
#		endif
#	endif
#endif
#ifndef platform_is_known

typedef unsigned long long int __dev_t;
typedef unsigned long int __ino_t;
typedef unsigned int __mode_t;
typedef unsigned int __nlink_t;
typedef unsigned int __uid_t;
typedef unsigned int __gid_t;
typedef long int __off_t;
typedef long int __blksize_t;
typedef long int __blkcnt_t;
typedef time_t __time_t;
typedef long int __syscall_slong_t;
struct timespec
{
    __time_t tv_sec;
    __syscall_slong_t tv_nsec;
};
struct stat
{
      __dev_t st_dev;
      unsigned short int __pad1;
      __ino_t st_ino;
      __mode_t st_mode;
      __nlink_t st_nlink;
      __uid_t st_uid;
      __gid_t st_gid;
      __dev_t st_rdev;
      unsigned short int __pad2;
      __off_t st_size;
      __blksize_t st_blksize;
      __blkcnt_t st_blocks;
      struct timespec st_atim;
      struct timespec st_mtim;
      struct timespec st_ctim;
      unsigned long int __glibc_reserved4;
      unsigned long int __glibc_reserved5;
};

#else
#	undef platform_is_known
#endif

#ifdef __cplusplus
extern "C" {
#endif

int mkdir(const char*, __mode_t);
int stat(const char *restrict pathname, struct stat *restrict statbuf);

#ifdef __cplusplus
}
#endif
