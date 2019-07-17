/*
	About: License

		Whatever

	Authors:

		<Razvan Raducu: https://twitter.com/Razvieu>

		<Ricardo J. Rodríguez: https://twitter.com/RicardoJRdez>
		
	Compilation:
		Simply use 'make' from the root directory. If you want debug messages 
		compile with 'make debug'
*/

#ifndef LIBTTHWART_HOOKED_FUNCTIONS_H_
#define LIBTTHWART_HOOKED_FUNCTIONS_H_

#include <stdio.h>
#include <sys/types.h>
#include <utime.h>
#include <sys/time.h>
#include <stdarg.h>

/*
	You can find here the declaration of every single pointer to the original
	functions that are being hooked. We need pointers to original functions
	because after all the hooking and checkin process happens, we need to call
	the original function in order to make this library transparent to the user.

	We devide functions in two blocks based on [1]:
	- Given a "vulnerable sequence" <checkFunction, useFunction> we have:
		Functions that are on the left size of the sequence (only check).
			These functions will only upsert information into the file metadata
			structure. 

		Functions that are on each or both sides of the sequence, either check, 
		use or both.
			These functions will either upsert information into the file metadata
			structure or will perform all the corresponding checkings given a path
			and inode. 


	[1] Modeling and preventing TOCTTOU vulnerabilities in Unix-style file systems.
			J Wei, C Pu - Computers & Security, 2010 - Elsevier
*/

// Left-handed functions <checkFuncion>
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


// Both-handed functions <useFunction>
extern int (*original_symlink)(const char *oldpath, const char *newpath);

extern int (*original_symlinkat)(const char *oldpath, int newdirfd, const char *newpath);

extern int (*original_link)(const char *oldpath, const char *newpath);

extern int (*original_linkat)(int olddirfd, const char *oldpath, int newdirfd, const char *newpath, int flags);

extern int (*original_rename)(const char *oldpath, const char *newpath);

extern int (*original_renameat)(int olddirfd, const char *oldpath, int newdirfd, const char *newpath);

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

extern int (*original_execve)(const char *pathname, char *const argv[], char *const envp[]);

extern int (*original_execvpe)(const char *file, char *const argv[], char *const envp[]);

extern FILE *(*original_popen)(const char *command, const char *type);

extern int (*original_mount)(const char *source, const char *target, const char *filesystemtype, unsigned long mountflags, const void *data);



/* execl* family is being hooked but there is no need for particular pointers 

	to functions since they internally call execv, execvp and execve respectively*/

//extern int (*original_execl)(const char *pathname, const char *arg, ...);

//extern int (*original_execlp)(const char *file, const char *arg, ...);

//extern int (*original_execle)(const char *pathname, const char *arg, ...);

//extern int (*original_execv)(const char *pathname, char *const argv[]);

//extern int (*original_execvp)(const char *file, char *const argv[]);


/*
    Checks properties of the given parameters, this is, the given path and 
    inode. Checking properties in this context means checking if a 
    file_object_info with the same path already exists in the array. If it
    doesn't, insert it; othwerwise (if it does) compare 
    the given inode and the inode of the file_object_info as well as the
    file mode and the device id. If they're equal continue execution; otherwise
    abort execution because a possible TOCTTOU is detected.

    Parameters:
    	Path: absolute path of the file whose metadata must be checked.
    	Caller_function_name: name of the function from which the checking was
    		called. 

    Checking parameters is different based on the FILE SYSTEM of the given path.
    At the moment, the function only considers two file systems:
    	EXT2/EXT3/EXT4 (their magic number is the same). When the path is located
    	in a ext2/3/4 file system, temporal hardlinks are created in order to
    	ensure the inode is not reused. One "feature" of such filesystems is that 
    	they reuse inode as soon as a given inode has 0 links pointing to it. 

    	OTHER FS. When the path is allocated on any other possible file system
    	the checkings performed are the same with the exception of temporal hard
    	links creation. We do not need hard links because inodes are not reused. 

    In order to see this function initialization, please refer to >get_fs_and_initialize_checking_functions>
*/
void check_parameters_properties(const char *, const char *);

/*
	Function to get full path of a given parameter without resolving, expanding
	symbolic links. That's why realpath() is useless. 
	Based on: https://stackoverflow.com/questions/4774116/realpath-without-resolving-symlinks/34202207#34202207
*/
const char * sanitize_and_get_absolute_path(const char *);

/*
	Function to get full path of a given parameter without resolving, expanding
	symbolic links but using a directory file descriptor as current working dir. 
	It is assumed that the file is indeed within that directory. The function 
	translates the file descriptor into the actual directory (string).
*/
const char * sanitize_and_get_absolute_path_from_dir_file_descriptor(const char *, int);

/*
	Function used to retrieve as string the full directory path pointed to
	by a given file descriptor. 
*/
char * get_directory_from_fd(int);

#endif