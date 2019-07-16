#ifndef LIBTTHWART_WRAPPERS_H
#define LIBTTHWART_WRAPPERS_H

#include <stdarg.h>
#include <stdio.h>

/*
	All the wrappers defined here are used for function calls within the library.
	The use of wrappers is very important because it avoids recursive calls of
	whatever function. For example, let's say there is a need to call open within
	the library. If the libarary were to call open(), since open() is a hooked
	function, the call would be recursive. We need to directly call original
	functions from within the library and we achieve that by using wrappers.
*/

/*
    The correct way to test for an error is to call dlerror() 
    to clear any old error conditions, then call dlsym(), and 
    then call dlerror() again, saving its return value into a
    variable, and check whether this saved value is not NULL.
    https://linux.die.net/man/3/dlsym
*/
extern void * dlsym_wrapper(const char *);

/*
    The open wrapper guarantees, insures original_open is initialized.
    It's used by other inner functions in order to avoid open() recursivity
    and overhead. In adittion, it deals with ellipsis (variable 
    arguments) since open is a variadic function.
*/
extern int open_wrapper(const char *, int, va_list);

extern int open64_wrapper(const char*, int, va_list);

/*
	Openat wrapper is exactly the same as <open_wrapper> but for
	openat.
*/
extern int openat_wrapper(int, const char *, int, va_list);

/*
	Wrapper for all execlX functions family. This wrapper treats the variable
	arguments and calls the corresponding execlX function according to:
	[function argument value] : [execlX function]
	0 : execl https://code.woboq.org/userspace/glibc/posix/execl.c.html
	1 : execlp https://code.woboq.org/userspace/glibc/posix/execlp.c.html
	2 : execle https://code.woboq.org/userspace/glibc/posix/execle.c.html

	Additional info: https://code.woboq.org/userspace/glibc/posix/execl.c.html
*/
extern int execlX_wrapper(int, const char *, const char *, va_list);

extern int execv_wrapper(const char *, char * const *);

extern int execvp_wrapper(const char *, char * const *);

extern int execve_wrapper(const char *, char * const *, char * const *);

extern int execvpe_wrapper(const char *, char * const *, char * const *);

extern int chdir_wrapper(const char *);

extern FILE* fopen_wrapper(const char*, const char*);

extern int mkdir_wrapper(const char*, mode_t);

extern int symlink_wrapper(const char *, const char *);

extern int link_wrapper(const char *, const char *);

extern int remove_wrapper(const char *);

#endif