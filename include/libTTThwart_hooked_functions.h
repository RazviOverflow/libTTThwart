#ifndef LIBTTHWART_HOOKED_FUNCTIONS_H_
#define LIBTTHWART_HOOKED_FUNCTIONS_H_

#include <stdio.h>
#include <sys/types.h>
#include <utime.h>
#include <sys/time.h>
#include <stdarg.h>

/// ########## Hooked functions ##########

// Left-handed functions
extern int (*original_xstat)(int ver, const char *path, struct stat *buf);
extern int (*original_xstat64)(int ver, const char *path, struct stat64 *buf);
extern int (*original_lxstat)(int ver, const char *path, struct stat *buf);
extern int (*original_lxstat64)(int ver, const char *path, struct stat64 *buf);
extern int (*original_access)(const char *path, int mode);
extern int (*original_rmdir)(const char *path);
extern int (*original_unlink)(const char *path);
extern int (*original_unlinkat)(int dirfd, const char *path, int flags);
extern int (*original_remove)(const char *path);
extern ssize_t (*original_readlink)(const char *pathname, char *buf, size_t bufsiz); // readlink(2)
extern ssize_t (*original_readlinkat)(int dirfd, const char *pathname, char *buf, size_t bufsiz);

// Both-handed functions
extern int (*original_symlink)(const char *oldpath, const char *newpath);
extern int (*original_symlinkat)(const char *oldpath, int newdirfd, const char *newpath);
extern int (*original_link)(const char *oldpath, const char *newpath);
extern int (*original_linkat)(int olddirfd, const char *oldpath, int newdirfd, const char *newpath, int flags);
extern int (*original_rename)(const char *oldpath, const char *newpath);
extern int (*original_renameat)(int olddirfd, const char *oldpath, int newdirfd, const char *newpath);
//extern int (*original_renameat2)(int olddirfd, const char *oldpath, int newdirfd, const char *newpath, unsigned int flags);
extern int (*original_creat64)(const char *path, mode_t mode);
extern int (*original_creat)(const char *path, mode_t mode);
extern int (*original_open)(const char *path, int flags, ...); 
extern int (*original_open64)(const char *path, int flags, ...); 
extern int (*original_openat)(int dirfd, const char *path, int flags, ...);
extern FILE *(*original_fopen)(const char *path, const char *mode);
extern FILE *(*original_fopen64)(const char *path, const char *mode);
extern FILE *(*original_freopen)(const char *pathname, const char *mode, FILE *stream);
extern int (*original_xmknod)(int ver, const char *path, mode_t mode, dev_t *dev);
extern int (*original_xmknodat)(int ver, int dirfd, const char *path, mode_t mode, dev_t *dev);
extern int (*original_mkfifo)(const char *pathname, mode_t mode);
extern int (*original_mkfifoat)(int dirfd, const char *pathname, mode_t mode);
extern int (*original_chmod)(const char *pathname, mode_t mode);
extern int (*original_chown)(const char *pathname, uid_t owner, gid_t group);
extern int (*original_truncate)(const char *path, off_t length);
extern int (*original_truncate64)(const char *path, off64_t length);
extern int (*original_utime)(const char *filename, const struct utimbuf *times);
extern int (*original_utimes)(const char *filename, const struct timeval times[2]);
extern long(*original_pathconf)(const char *path, int name);
extern int (*original_mkdir)(const char *pathname, mode_t mode);
extern int (*original_mkdirat)(int dirfd, const char *pathname, mode_t mode);
extern int (*original_chdir)(const char *path);
extern int (*original_chroot)(const char *path);
//extern int (*original_pivot_root)(const char *new_root, const char *putold);

/* execl* family is being hooked but there is no need for particular pointers 
	to functions since they internally call execv, execvp and execve respectively*/
//extern int (*original_execl)(const char *pathname, const char *arg, ...);
//extern int (*original_execlp)(const char *file, const char *arg, ...);
//extern int (*original_execle)(const char *pathname, const char *arg, ...);

//extern int (*original_execv)(const char *pathname, char *const argv[]);
//extern int (*original_execvp)(const char *file, char *const argv[]);
extern int (*original_execve)(const char *pathname, char *const argv[], char *const envp[]);
extern int (*original_execvpe)(const char *file, char *const argv[], char *const envp[]);

// doubts
extern FILE *(*original_popen)(const char *command, const char *type);
//extern int (*original_pclose)(FILE *stream);
extern int (*original_mount)(const char *source, const char *target, const char *filesystemtype, unsigned long mountflags, const void *data);

#endif