#ifndef LIBTTHWART_WRAPPERS_H
#define LIBTTHWART_WRAPPERS_H

#include <stdarg.h>
#include <stdio.h>

/// ########## Wrappers ##########

extern void * dlsym_wrapper(const char *);

extern int open_wrapper(const char *, int, va_list);

extern int open64_wrapper(const char*, int, va_list);

extern int openat_wrapper(int, const char *, int, va_list);

extern int execlX_wrapper(int, const char *, const char *, va_list);

extern int execv_wrapper(const char *, char * const *);

extern int execvp_wrapper(const char *, char * const *);

extern int execve_wrapper(const char *, char * const *, char * const *);

extern int execvpe_wrapper(const char *, char * const *, char * const *);

extern int chdir_wrapper(const char *);

extern FILE* fopen_wrapper(const char*, const char*);

/// <-------------------------------------------------> 

#endif