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
#include "libTTThwart_wrappers.h"


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
    Retrieves the corresponding inode of a given path. If path is a symlink
    it retrieves the inode of the target rather than the symlink itself. 
*/
ino_t get_inode(const char *path){
	int fd;
	ino_t inode;

	fd = open_wrapper(path, O_RDONLY, NULL);
	if (fd < 0){
		if(errno == EMFILE){
			zlogf_time(ZLOG_INFO_LOG_MSG, "[!] Errors occurred while getting inode of %s.\n[!] The per-process limit on the number of open file descriptors has been reached.\n[!] ERROR: %s\n", path, strerror(errno));
			close(fd);
			return 0;
		} else if (errno == ENFILE){
			zlogf_time(ZLOG_INFO_LOG_MSG, "[!] Errors occurred while getting inode of %s.\n[!] The system-wide limit on the total number of open files has been reached.\n[!] ERROR: %s\n", path, strerror(errno));
			close(fd);
			return 0;
		} else {
			zlogf_time(ZLOG_INFO_LOG_MSG, "[!] Errors occurred while getting inode of %s. ERROR: %s\n", path, strerror(errno));
			close(fd);
			return 0;
		}
	}
	struct stat file_stat;
	fstat(fd, &file_stat);
	inode = file_stat.st_ino; 
	close(fd);

	return inode;
}



/*
	Function to get full path of a given parameter without resolving, expanding
	symbolic links. That's why realpath() is useless. 
	Based on: https://stackoverflow.com/questions/4774116/realpath-without-resolving-symlinks/34202207#34202207
*/
const char * sanitize_and_get_absolute_path(const char * src) {

		char *res;
        size_t res_len;
        size_t src_len = strlen(src);

        const char *pointer;
        const char *end_pointer;
        const char *next_pointer;

        // Relative path
        if (src_len == 0 || src[0] != '/') {

                char cwd[PATH_MAX];
                size_t cwd_len;

                // Copy into cwd the null-terminated current working 
                // directory with a max length of sizeof(cwd)
                if (getcwd(cwd, sizeof(cwd)) == NULL) {
                        return NULL;
                }

                cwd_len = strlen(cwd);
                res = malloc(cwd_len + 1 + src_len + 1);
                // Copies cwd_len bytes from cwd to res
                memcpy(res, cwd, cwd_len);
                res_len = cwd_len;
                
        } else {
        // Absolute path
                //res = malloc((src_len > 0 ? src_len : 1) + 1);
        		res = malloc(src_len + 1);
                res_len = 0;
                
        }

        end_pointer = &src[src_len];


        for (pointer = src; pointer < end_pointer; pointer =next_pointer+1) {
                size_t len;

                // Scans the initial end_pointer-pointer bytes of the memory area pointed 
                // to by pointer for the first instance of '/'
                next_pointer = memchr(pointer, '/', end_pointer-pointer);

                if (next_pointer == NULL) {
                        next_pointer = end_pointer;
                }


                len = next_pointer-pointer;


                switch(len) {
                case 2:
                        if (pointer[0] == '.' && pointer[1] == '.') {
                        	// memrchr is like memchr, except that it searches 
                        	// backward from the end of the res_len bytes pointed
                        	// to by res instead of forward from the beginning
                                const char * slash = memrchr(res, '/', res_len);
                                if (slash != NULL) {
                                	// This way the last node from the current
                                	// directory is deleted. Lets say res starts
                                	// @ 0x2 mem address and slash is @ 0x10.
                                	// res_len would be 0x8 which is exactly
                                	// the length between 0x2 and 0x10.
                                        res_len = slash - res;
                                }
                                // Continue applies only to loop statements. 
                                //That is, this jumps right to next for iteration,
                                // skipping the remaining code.
                                continue; 
                        }
                        break;
                case 1:
                        if (pointer[0] == '.') {
                                continue;

                        }
                        break;
                case 0:
                        continue;
                }
                res[res_len++] = '/';
                memcpy(&res[res_len], pointer, len);
                res_len += len;
        }

        if (res_len == 0) {
                res[res_len++] = '/';
        }
        // Marks the end of the new sanitized and absoluted path, thus discarding
        // whatever follows res_len
        res[res_len] = '\0';

        return res;
}

/*
	Function to get full path of a given parameter without resolving, expanding
	symbolic links but using a directory file descriptor as current working dir. It is assumed that the file 
	is indeed within that directory. The function translates the file descriptor
	into the actual directory (string).
*/
const char * sanitize_and_get_absolute_path_from_dir_file_descriptor(const char *src, int directory_fd) {

		// changing current working directory
        char *original_working_dir = get_current_dir_name();
		fchdir(directory_fd);


		const char *res = sanitize_and_get_absolute_path(src);

        // Restoring working dir
        chdir_wrapper(original_working_dir);
		free (original_working_dir);

        return res;
}

/*
	Function used to determine whether a path is absolute. If it isn't it's, 
	obviosuly, because it is relative.
*/
bool path_is_absolute(const char *path){
	return (path[0] == '/');
}

int file_does_exist(const char *pathname){
	
	int fd = open_wrapper(pathname, O_RDONLY, NULL);

	if( fd < 0){
		close(fd);
		return 0;
	} else {
		close(fd);
		return 1;
	}
	
}

/*
	Function used to retrieve as string the full directory path pointed to
	by a given file descriptor. 
*/
char * get_directory_from_fd(int directory_fd){
	char *original_working_dir = get_current_dir_name();

	fchdir(directory_fd);

	char *directory_fd_path = get_current_dir_name();

	chdir_wrapper(original_working_dir);

	free (original_working_dir);

	return directory_fd_path;
}

