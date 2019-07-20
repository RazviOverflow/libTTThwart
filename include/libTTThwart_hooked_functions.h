/*
	About: License

		Copyright 2019 Razvan Raducu and Ricardo J. Rodriguez

		This file is part of libTTThwart.so.

	    libTTThwart.so is free software: you can redistribute it and/or modify
	    it under the terms of the GNU General Public License as published by
	    the Free Software Foundation, either version 3 of the License, or
	    (at your option) any later version.

	    libTTThwart.so is distributed in the hope that it will be useful,
	    but WITHOUT ANY WARRANTY; without even the implied warranty of
	    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	    GNU General Public License for more details.

	    You should have received a copy of the GNU General Public License
	    along with libTTThwart.so.  If not, see <https://www.gnu.org/licenses/>.

	Authors:

		<Razvan Raducu: https://twitter.com/Razvieu>

		<Ricardo J. Rodríguez: https://twitter.com/RicardoJRdez>
		
	Code: 
		You can find the source code associated with this header <here: https://github.com/RazviOverflow/ld.so.preload/blob/master/src/libTTThwart_hooked_functions.c>.

*/

#ifndef LIBTTHWART_HOOKED_FUNCTIONS_H_
#define LIBTTHWART_HOOKED_FUNCTIONS_H_

#include <stdio.h>
#include <sys/types.h>
#include <utime.h>
#include <sys/time.h>
#include <stdarg.h>

/*	
	About: General Information

	You can find in this file the declaration of every single *pointer to* the original
	functions that are being hooked. We need pointers to original functions
	because after all the hooking and checkin process happens, we need to call
	the original function in order to make this library transparent to the user.

	We devide functions in two blocks based on [1]:

	- Given a "vulnerable sequence" <checkFunction, useFunction> we have:
			- Functions that are *only on the left part of the vulnerable sequence* (only check).
			These functions will only upsert information into the file metadata
			structure. 

			- Functions that are *either on the left or rgiht (both sides) of the 
			vulnerable sequence, either check, use or both.*
			These functions will either upsert information into the file metadata
			structure or will perform all the corresponding checkings given a path
			and inode. 


	[1] Modeling and preventing TOCTTOU vulnerabilities in Unix-style file systems.
			J Wei, C Pu - Computers & Security, 2010 - Elsevier

	_Please notice how there are no function that are only on the right part, the
	 use side, of the vulnerable sequence._
*/

/*
	Title: Hooked functions

	Variables: Left-handed hooked functions
		original_xstat - Function pointer to original stat() function. 
		original_xstat64 - Function pointer to original stat64() function. 
		original_lxstat - Function pointer to original lstat() function.
		original_lxtat64 - Function pointer to original lstat64() function.
		original_access - Function pointer to original access() function.
		original_rmdir - Function pointer to original rmdir() function.
		original_unlink - Function pointer to original unlink() function.
		original_unlinkat - Function pointer to original unlinkat() function.
		original_remove - Function pointer to original remove() function.
		original_readlink - Function pointer to original readlink() function.
		original_readlinkat - Function pointer to original readlinkat() function.

*/
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

/*
	Variables: Both-handed hooked functions
		original_symlink - Function pointer to original symlink() function. 
		original_symlinkat - Function pointer to original symlinkat() function. 
		original_link - Function pointer to original link() function. 
		original_linkat - Function pointer to original linkat() function. 
		original_rename - Function pointer to original rename() function. 
		original_renameat - Function pointer to original renameat() function. 
		original_creat64 - Function pointer to original creat64() function. 
		original_creat - Function pointer to original creat() function. 
		original_open - Function pointer to original open() function. 
		original_open64 - Function pointer to original open64() function. 
		original_openat - Function pointer to original openat() function. 
		original_fopen - Function pointer to original fopen() function. 
		original_fopen64 - Function pointer to original fopen64() function. 
		original_freopen - Function pointer to original freopen() function. 
		original_xmknod - Function pointer to original xmknode() function. 
		original_xmknodat - Function pointer to original xmknodat() function. 
		original_mkfifo - Function pointer to original mkfifo() function. 
		original_mkfifoat - Function pointer to original mkfifoat() function. 
		original_chmod - Function pointer to original chmod() function. 
		original_chown - Function pointer to original chown() function. 
		original_truncate - Function pointer to original truncate() function. 
		original_truncate64 - Function pointer to original truncate64() function. 
		original_utime - Function pointer to original utime() function. 
		original_utimes - Function pointer to original utimes() function. 
		original_pathconf - Function pointer to original pathconf() function. 
		original_mkdir - Function pointer to original mdkir() function. 
		original_mkdirat - Function pointer to original mkdirat() function. 
		original_chdir - Function pointer to original chdir() function. 
		original_chroot - Function pointer to original chroot() function. 
		original_execve - Function pointer to original execve() function. 
		original_execvpe - Function pointer to original execvpe() function. 
		original_popen - Function pointer to original popen() function. 
		original_mount - Function pointer to original mount() function. 
*/
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

/* 

	Variables: execl* family
	
	execl* family is being hooked but there is no need for particular function 
	pointers since they internally call execv, execvp and execve respectively

*/
//extern int (*original_execl)(const char *pathname, const char *arg, ...);
//extern int (*original_execlp)(const char *file, const char *arg, ...);
//extern int (*original_execle)(const char *pathname, const char *arg, ...);
//extern int (*original_execv)(const char *pathname, char *const argv[]);
//extern int (*original_execvp)(const char *file, char *const argv[]);

#endif