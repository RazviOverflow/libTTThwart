/*
	Razvi Overflow

	Testing-purposes library. Hooking file management functions. 
*/

#define _GNU_SOURCE

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <dlfcn.h>

static void before_main(void) __attribute__((constructor));
static int (*old_unlink)(const char *path) = NULL;

void* dlsym_wrapper(const char *);

extern char *program_invocation_name;

static void before_main(void){
	printf("I'm process %s and my PID is %d\n", program_invocation_name, getpid());
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


int unlink(const char *path){

	printf("Process %s with pid %d called unlink for path %s\n", program_invocation_name, getpid(), path);

	if(old_unlink == NULL){
		old_unlink = dlsym_wrapper(__func__);
	}

	return old_unlink(path);

}
