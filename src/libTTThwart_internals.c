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

#include "libTTThwart_internals.h"
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
	printf("I REACH %s. THEY CALLED FIRST: %s\n", __func__, func);
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




