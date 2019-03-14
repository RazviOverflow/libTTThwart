/* RazviOverflow
 This file must be compiled with the following command:
	gcc -shared -fPIC liblstathooking.c -o liblstathooking.so -ldl
 You can then execute the vulnerable code with:
 	LD_PRELOAD=$PWD/liblstathooking.so ./vulnerable tryout

Iint lstat64 (const char *__restrict __file
*/
#define _GNU_SOURCE

#include <stdio.h>
#include <dlfcn.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

static int (*old_xstat)(int ver, const char *path, struct stat *buf) = NULL;
static int (*old_lxstat)(int ver, const char *path, struct stat *buf) = NULL;
static int (*old_xstat64)(int ver, const char *path, struct stat64 *buf) = NULL;
static int (*old_open)(const char *path, int flags);

int __xstat(int ver, const char *path, struct stat *buf)
{
  if ( old_xstat == NULL ) {
    old_xstat = dlsym(RTLD_NEXT, "__xstat");
  }

  printf("xstat %s\n",path);
  return old_xstat(ver,path, buf);
} 

int __lxstat(int ver, const char *path, struct stat *buf)
{

  printf("User program invoked lstat on file %s, ver number is %d\n", path, ver);
  
  printf("Address of path is: %X\n", &path);

  if ( old_lxstat == NULL ) {
    old_lxstat = dlsym(RTLD_NEXT, "__lxstat");
  }

  printf("lxstat %s\n",path);
  return old_lxstat(ver,path, buf);
}

int __xstat64(int ver, const char *path, struct stat64 *buf)
{
  if ( old_xstat64 == NULL ) {
    old_xstat64 = dlsym(RTLD_NEXT, "__xstat64");
  }

  printf("xstat64 %s\n",path);
  return old_xstat64(ver,path, buf);
}

int open(const char *path, int flags)
{

	printf("User program invoked open on file %s, with flags %d\n", path, flags);
  
  printf("Address of path is: %X\n", &path);

  if ( old_open == NULL ) {
    old_open = dlsym(RTLD_NEXT, "open");
  }

  printf("open %s\n",path);
  return old_open(path, flags);
}