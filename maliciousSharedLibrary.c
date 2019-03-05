/* RazviOverflow
 This file must be compiled with the following command:
	gcc -shared -fPIC maliciousSharedLibrary.c -o maliciousSharedLibrary.so -ldl
 You can then execute the vulnerable code with:
 	LD_PRELOAD=$PWD/maliciousSharedLibrary.so vulnearble
*/
#define _GNU_SOURCE

#include <stdio.h>
#include <sys/stat.h>
//Needed for retrieving symbols fomr the dynamic linker. 
#include <dlfcn.h>

typedef int (*real_lstat_type)(const char *path, struct stat *buf);
typedef int (*real_open_type)(const char *path, int flags);

int lstat(const char *path, struct stat *buf){

	printf("User invoked lstat for file %s", path);
	real_lstat_type real_lstat = (real_lstat_type) dlsym(RTLD_NEXT,"lstat");

	return real_lstat(path, buf);
}

int open(const char *path, int flags){
	printf("User invoked open for file %s", path);
	real_open_type real_open = (real_open_type) dlsym(RTLD_NEXT, "open");

	return real_open(path, flags);
}