#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <time.h>
#include <unistd.h>
#include <dlfcn.h>
#include <stddef.h>
#include <limits.h>
#include <ftw.h>

#include "libTTThwart_internals.h"
#include "libTTThwart_wrappers.h"
#include "zlog.h"

#undef GET_PROGRAM_NAME
#ifdef __GLIBC__
#   define GET_PROGRAM_NAME() program_invocation_short_name
#elif defined(__APPLE__) || defined(__FreeBSD__)
#   define GET_PROGRAM_NAME() getprogname()
#else 
#	define GET_PROGRAM_NAME() "?";
#endif

void check_dlsym_error(){
	char * error = dlerror();
	if(error != NULL){
		exit(EXIT_FAILURE);
	}
}

void print_function_and_path(const char* func, const char* path, const char* sanitized_path){
	zlogf_time(ZLOG_DEBUG_LOG_MSG, "[+] User invoked %s via process %s on: %s\n", func, GET_PROGRAM_NAME(), path);
	zlogf_time(ZLOG_DEBUG_LOG_MSG, "[+] Sanitized: %s\n", sanitized_path);
}


int get_number_of_variable_arguments_char_pointer_type(va_list variable_arguments){
	ptrdiff_t number_of_arguments;

	for(number_of_arguments = 1; va_arg(variable_arguments, const char *); number_of_arguments++){
		if(number_of_arguments == INT_MAX){
			va_end(variable_arguments);
			errno = E2BIG;
			return -1;
		}
	}

	return number_of_arguments;
}

/*
	Function used to determine whether a path is absolute. If it isn't it's, 
	obviosuly, because it is relative.
*/
bool path_is_absolute(const char *path){
	return (path[0] == '/');
}

// __attribute__ ((unused)) Just to indicate GCC to not throw -Wunused-parameter warnings. 
int unlink_recursively(const char *fpath, const struct stat *sb __attribute__ ((unused)), int typeflag __attribute__ ((unused)), struct FTW *ftwbuf __attribute__ ((unused))){
    int remove_result = remove_wrapper(fpath);

    if(remove_result == -1){
    	zlogf_time(ZLOG_DEBUG_LOG_MSG, "[!] ERROR while removing recursively temporal file: %s\n[!] ERROR: %s\n", fpath, strerror(errno));
    }

    return remove_result;
}

int remove_directory_and_content(char *path_to_remove){
	return nftw(path_to_remove, unlink_recursively, 32, FTW_DEPTH | FTW_PHYS);
}


