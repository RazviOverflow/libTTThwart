#ifndef LIBTTHWART_WRAPPERS_H_
#define LIBTTHWART_WRAPPERS_H_

#include <stdarg.h>

void * dlsym_wrapper(const char *);

int open_wrapper(const char *, int, va_list);

int open64_wrapper(const char*, int, va_list);

int openat_wrapper(int, const char *, int, va_list);

int execlX_wrapper(int, const char *, const char *, va_list);

int execv_wrapper(const char *, char * const *);

int execvp_wrapper(const char *, char * const *);

int execve_wrapper(const char *, char * const *, char * const *);

int execvpe_wrapper(const char *, char * const *, char * const *);

int chdir_wrapper(const char *);

#endif