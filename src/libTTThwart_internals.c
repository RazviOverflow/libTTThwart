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
#include <sys/vfs.h>

#include "/usr/include/linux/magic.h" //Included for filesystem signatures

#include "libTTThwart_internals.h"
#include "libTTThwart_wrappers.h"
#include "libTTThwart_file_objects_info.h"
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

void get_fs_and_initialize_checking_functions(const char *path){

	if(file_does_exist(path)){
		struct statfs struct_statfs;

		if(statfs(path, &struct_statfs) == -1){
			zlogf_time(ZLOG_DEBUG_LOG_MSG, "[!] ERROR while retrieving filesystem of: %s\n[!] ERROR: %s\n[!] ABORTING\n", path, strerror(errno));
			fprintf(stderr,"[!] ERROR while retrieving filesystem of: %s\n[!] ERROR: %s\n[!] ABORTING\n", path, strerror(errno));
			exit(EXIT_FAILURE);

		}

		/*
			EXT2, EXT3 and EXT4 have the same value.
			(from http://man7.org/linux/man-pages/man2/statfs.2.html)
		  	EXT2_SUPER_MAGIC      0xef53
	       	EXT3_SUPER_MAGIC      0xef53
	       	EXT4_SUPER_MAGIC      0xef53
		*/

		switch(struct_statfs.f_type){
			case EXT2_SUPER_MAGIC: 
				upsert_file_data_in_array = upsert_file_data_in_array_ext3ext4;
				break;
			default:
				upsert_file_data_in_array = upsert_file_data_in_array_otherfs;
				break;
		}
	} else {
		upsert_file_data_in_array = upsert_file_data_in_array_ext3ext4;
	}

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

struct stat get_file_metadata(const char *path){
	int fd;

	fd = open_wrapper(path, O_RDONLY, NULL);
	if (fd < 0){
		if(errno == EMFILE){
			zlogf_time(ZLOG_INFO_LOG_MSG, "[!] Errors occurred while getting file metadata of %s.\n[!] The per-process limit on the number of open file descriptors has been reached.\n[!] ERROR: %s\n", path, strerror(errno));
		} else if (errno == ENFILE){
			zlogf_time(ZLOG_INFO_LOG_MSG, "[!] Errors occurred while getting file metadata of %s.\n[!] The system-wide limit on the total number of open files has been reached.\n[!] ERROR: %s\n", path, strerror(errno));
		} else {
			zlogf_time(ZLOG_INFO_LOG_MSG, "[!] Errors occurred while getting file metadata of %s. ERROR: %s\n", path, strerror(errno));
		}
	}

	struct stat file_stat;
	fstat(fd, &file_stat);
	close(fd);

	return file_stat;

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

bool starts_with(const char *pre, const char *str){
    size_t lenpre = strlen(pre),
           lenstr = strlen(str);
    return lenstr < lenpre ? false : strncmp(pre, str, lenpre) == 0;
}