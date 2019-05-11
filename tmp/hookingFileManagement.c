/*
	Razvi Overflow

	Testing-purpose library. Hooking file management functions. 
*/

#define _GNU_SOURCE

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <dlfcn.h>
#include <stdarg.h>

static void before_main(void) __attribute__((constructor));

static int (*original_unlink)(const char *path) = NULL;
static int (*original_openat)(int dirfd, const char *path, int flags, ...) = NULL;
static int (*original_unlinkat)(int dirfd, const char *path, int flags) = NULL;
static int (*original_symlink)(const char *oldpath, const char *newpath) = NULL;
static int (*original_symlinkat)(const char *oldpath, int newdirfd, const char *newpath) = NULL;
static int (*original_remove)(const char *path) = NULL;
static int (*original_mknod)(const char *path, mode_t mode, dev_t dev) = NULL;
static int (*original_xmknod)(int ver, const char *path, mode_t mode, dev_t *dev) = NULL;
static int (*original_xmknodat)(int ver, int dirfd, const char *path, mode_t mode, dev_t *dev) = NULL;
static int (*original_link)(const char *oldpath, const char*newname) = NULL;
static int (*original_linkat)(int olddirfd, const char *oldpath, int newdirfd, const char *newpath, int flags) = NULL;
static int (*original_creat64)(const char *path, mode_t mode) = NULL;
static int (*original_creat)(const char *path, mode_t mode) = NULL;

void* dlsym_wrapper(const char *);
int openat_wrapper(int, const char *, int, va_list argptr);

extern char *program_invocation_name;

static void before_main(void){
	printf("I'm process %s and my PID is %d\n", program_invocation_name, getpid());
}

int openat_wrapper(int dirfd, const char *path, int flags, va_list argptr){

	if(original_openat == NULL){
		original_openat = = dlsym_wrapper("openat");
	}

	return original_openat(dirfd, path, flags, argptr);

}
void check_dlsym_error(){
	char * error = dlerror();
	if(error != NULL){
		//printf("There were errors while retrieving the original function from the dynamic linker/loader.\nDlsym error: %s\n", error);
		exit(EXIT_FAILURE);
	}
}

void* dlsym_wrapper(const char *original_function){

	dlerror();

	void *function_handler;

	function_handler = dlsym(RTLD_NEXT, original_function);

	check_dlsym_error();

	return function_handler;
}

int openat(int dirfd, const char *path, int flags, ...){
	printf("Process %s with pid %d called %s for path %s\n", program_invocation_name, getpid(), __func__, path);

	va_list variable_arguments;
	va_start(variable_arguments, flags);

	int openat_result =  openat_wrapper(dirfd, path, flags, variable_arguments);

	va_end(variable_arguments);
	return openat_result;

}

int unlink(const char *path){

	printf("Process %s with pid %d called %s for path %s\n", program_invocation_name, getpid(), __func__, path);

	if(original_unlink == NULL){
		original_unlink = dlsym_wrapper(__func__);
	}

	return original_unlink(path);

}

int unlinkat(int dirfd, const char *path, int flags){
	printf("Process %s with pid %d called %s for path %s\n", program_invocation_name, getpid(), __func__, path);

	if(original_unlinkat == NULL){
		original_unlinkat = dlsym_wrapper(__func__);
	}

	return original_unlinkat(dirfd, path, flags);

}

int symlink(const char *oldpath, const char *newpath){
    printf("Process %s with pid %d called %s for oldpath: %s and newpath: %s\n", program_invocation_name, getpid(), __func__, oldpath, newpath);

    if(original_symlink == NULL){
    	original_symlink = dlsym_wrapper(__func__);
    }

    return original_symlink(oldpath, newpath);

}

int symlinkat(const char *oldpath, int newdirfd, const char *newpath){
	printf("Process %s with pid %d called %s for oldpath: %s and newpath: %s\n", program_invocation_name, getpid(), __func__, oldpath, newpath);

	if(original_symlinkat == NULL){
		original_symlinkat = dlsym_wrapper(__func__);
	}

	return original_symlinkat(oldpath, newdirfd, newpath);

}

int remove(const char *path) {
    printf("Process %s with pid %d called %s for path %s\n", program_invocation_name, getpid(), __func__, path);

    if(original_remove == NULL){
    	original_remove = dlsym_wrapper(__func__);
    }

    return original_remove(path);

}

int mknod(const char *path, mode_t mode, dev_t dev){
    printf("Process %s with pid %d called %s for path %s\n", program_invocation_name, getpid(), __func__, path);

    if(original_mknod == NULL){
    	original_mknod = dlsym_wrapper(__func__);
    }

    return original_mknod(path, mode, dev);

}

int __xmknod(int ver, const char *path, mode_t mode, dev_t *dev){
    printf("Process %s with pid %d called %s for path %s\n", program_invocation_name, getpid(), __func__, path);

    if(original_xmknod == NULL){
    	original_xmknod = dlsym_wrapper(__func__);
    }

    return original_xmknod(ver, path, mode, dev);

}

int __xmknodat(int ver, int dirfd, const char *path, mode_t mode, dev_t *dev){
	printf("Process %s with pid %d called %s for path %s\n", program_invocation_name, getpid(), __func__, path);

	if(original_xmknodat == NULL){
		original_xmknodat = dlsym_wrapper(__func__);
	}

	return original_xmknodat(ver, dirfd, path, mode, dev);

}

int link(const char *oldpath, const char *newname){
   printf("Process %s with pid %d called %s for oldpath: %s and newname: %s\n", program_invocation_name, getpid(), __func__, oldpath, newname);

   if(original_link == NULL){
   		original_link = dlsym_wrapper(__func__);
   }

   return original_link(oldpath, newname);

}

int linkat(int olddirfd, const  char *oldpath, int newdirfd, const char *newpath, int flags){
	printf("Process %s with pid %d called %s for oldpath: %s and newpath: %s\n", program_invocation_name, getpid(), __func__, oldpath, newpath);

	if(original_linkat == NULL){
		original_linkat = dlsym_wrapper(__func__);
	}

	return original_linkat(olddirfd, oldpath, newdirfd, newpath, flags);

}

int creat64(const char *path, mode_t mode){
    printf("Process %s with pid %d called %s for path %s\n", program_invocation_name, getpid(), __func__, path);

    if(original_creat64 == NULL){
    	original_creat64 = dlsym_wrapper(__func__);
    }

    return original_creat64(path, mode);

}

int creat(const char *path, mode_t mode){
    printf("Process %s with pid %d called %s for path %s\n", program_invocation_name, getpid(), __func__, path);

    if(original_creat == NULL){
    	original_creat = dlsym_wrapper(__func__);
    }

    return original_creat(path, mode);

}

