/* RazviOverflow
 This file must be compiled with the following command:
	gcc -shared -Wall -Wextra -fPIC ALLinONE.c -o ALLinONE.so -std=c99 -ldl
 
 Glibc min version => 2.19
*/
#define _GNU_SOURCE

#include <dlfcn.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdbool.h>

#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <stdarg.h>
#include <stddef.h>

#include <limits.h>
#include <time.h>
#include <utime.h>
#include <sys/time.h>
#include <dirent.h>


// Logger
#include "zlog.h"
// Array definition and operationss
#include "libTTThwart_file_objects_info.h"
// Library internal operations
#include "libTTThwart_internals.h"
// Hooked Functions and wrappers for them
#include "libTTThwart_hooked_functions.h"
// Wrappers
#include "libTTThwart_wrappers.h"
// Global Variables
#include "libTTThwart_global_variables.h"


// Left-handed functions
 int (*original_xstat)(int ver, const char *path, struct stat *buf) = NULL;
 int (*original_xstat64)(int ver, const char *path, struct stat64 *buf) = NULL;
 int (*original_lxstat)(int ver, const char *path, struct stat *buf) = NULL;
 int (*original_lxstat64)(int ver, const char *path, struct stat64 *buf) = NULL;
 int (*original_access)(const char *path, int mode) = NULL;
 int (*original_rmdir)(const char *path) = NULL;
 int (*original_unlink)(const char *path) = NULL;
 int (*original_unlinkat)(int dirfd, const char *path, int flags) = NULL;
 int (*original_remove)(const char *path) = NULL;
 ssize_t (*original_readlink)(const char *pathname, char *buf, size_t bufsiz) = NULL; // readlink(2)
 ssize_t (*original_readlinkat)(int dirfd, const char *pathname, char *buf, size_t bufsiz);

// Both-handed functions
 int (*original_symlink)(const char *oldpath, const char *newpath) = NULL;
 int (*original_symlinkat)(const char *oldpath, int newdirfd, const char *newpath) = NULL;
 int (*original_link)(const char *oldpath, const char *newpath) = NULL;
 int (*original_linkat)(int olddirfd, const char *oldpath, int newdirfd, const char *newpath, int flags) = NULL;
 int (*original_rename)(const char *oldpath, const char *newpath) = NULL;
 int (*original_renameat)(int olddirfd, const char *oldpath, int newdirfd, const char *newpath) = NULL;
// int (*original_renameat2)(int olddirfd, const char *oldpath, int newdirfd, const char *newpath, unsigned int flags) = NULL;
 int (*original_creat64)(const char *path, mode_t mode) = NULL;
 int (*original_creat)(const char *path, mode_t mode) = NULL;
 int (*original_open)(const char *path, int flags, ...) = NULL; 
 int (*original_open64)(const char *path, int flags, ...) = NULL; 
 int (*original_openat)(int dirfd, const char *path, int flags, ...) = NULL;
 FILE *(*original_fopen)(const char *path, const char *mode) = NULL;
 FILE *(*original_fopen64)(const char *path, const char *mode) = NULL;
 FILE *(*original_freopen)(const char *pathname, const char *mode, FILE *stream) = NULL;
 int (*original_xmknod)(int ver, const char *path, mode_t mode, dev_t *dev) = NULL;
 int (*original_xmknodat)(int ver, int dirfd, const char *path, mode_t mode, dev_t *dev) = NULL;
 int (*original_mkfifo)(const char *pathname, mode_t mode) = NULL;
 int (*original_mkfifoat)(int dirfd, const char *pathname, mode_t mode) = NULL;
 int (*original_chmod)(const char *pathname, mode_t mode) = NULL;
 int (*original_chown)(const char *pathname, uid_t owner, gid_t group) = NULL;
 int (*original_truncate)(const char *path, off_t length) = NULL;
 int (*original_truncate64)(const char *path, off64_t length) = NULL;
 int (*original_utime)(const char *filename, const struct utimbuf *times) = NULL;
 int (*original_utimes)(const char *filename, const struct timeval times[2]) = NULL;
 long(*original_pathconf)(const char *path, int name) = NULL;
 int (*original_mkdir)(const char *pathname, mode_t mode) = NULL;
 int (*original_mkdirat)(int dirfd, const char *pathname, mode_t mode) = NULL;
 int (*original_chdir)(const char *path) = NULL;
 int (*original_chroot)(const char *path) = NULL;
// int (*original_pivot_root)(const char *new_root, const char *putold) = NULL;

/* execl* family is being hooked but there is no need for particular pointers 
	to functions since they internally call execv, execvp and execve respectively*/
// int (*original_execl)(const char *pathname, const char *arg, ...) = NULL;
// int (*original_execlp)(const char *file, const char *arg, ...) = NULL;
// int (*original_execle)(const char *pathname, const char *arg, ...) = NULL;

// int (*original_execv)(const char *pathname, char *const argv[]) = NULL;
// int (*original_execvp)(const char *file, char *const argv[]) = NULL;
 int (*original_execve)(const char *pathname, char *const argv[], char *const envp[]) = NULL;
 int (*original_execvpe)(const char *file, char *const argv[], char *const envp[]) = NULL;

// doubts
 FILE *(*original_popen)(const char *command, const char *type) = NULL;
// int (*original_pclose)(FILE *stream) = NULL;
 int (*original_mount)(const char *source, const char *target, const char *filesystemtype, unsigned long mountflags, const void *data) = NULL;
/// <-------------------------------------------------> 

/// ########## Coconstructor and Destructor ##########
static void before_main(void) __attribute__((constructor));
static void after_main(void) __attribute__((destructor));
/// ########## Coconstructor and Destructor ##########

/// <-------------------------------------------------> 

/// ########## Prototype declaration ##########
void create_log_dir_and_start_logger();
void create_log_file_and_start_logger(const char *);
void create_temp_dir();

/// ########## Prototype declaration ##########

/// <-------------------------------------------------> 


/// <------------------------------------------------->



/// <-------------------------------------------------> 

/// ########## Coconstructor and Destructor ##########

static void before_main(void){

	//printf("PATH Variable environment: %s\n", getenv("PATH"));
	//printf("ENVIRON: %s\n", *__environ);

	printf("I REACH %s\n", __func__);

	/*
	if((getuid() != geteuid()) || (getgid() != geteuid())){
		LIBRARY_ON = true;
	}
	*/

	LIBRARY_ON = true;

	if(LIBRARY_ON){

		create_log_dir_and_start_logger();
		create_temp_dir();

		//zlog_init_stdout();

		zlogf_time(ZLOG_DEBUG_LOG_MSG, "[+] I AM %s w/ PID: %d and PPID: %d [+]\n", GET_PROGRAM_NAME(), getpid(), getppid());
		zlog_flush_buffer();
	}

	printf("I GET OUT OF %s\n", __func__);

}

static void after_main(void){

	printf("I REACH %s\n", __func__);

	if(LIBRARY_ON){
		zlogf_time(ZLOG_DEBUG_LOG_MSG,"[+] I WAS  %s w/ PID: %d and PPID: %d [+]\n", GET_PROGRAM_NAME(), getpid(), getppid());
		zlog_flush_buffer();
	}
	free_array(&g_array);

	printf("I GET OUT OF %s\n", __func__);

}

/// ########## Coconstructor and Destructor ##########

/// <-------------------------------------------------> 

/// ########## Logger ##########


void create_log_dir_and_start_logger(){

	printf("I REACH %s\n", __func__);

	zlog_init_stdout();

	char *home = getenv("HOME");
	if(!home){
		fprintf(stderr, "[!] ERROR RETRIEVING HOME ENV VARIABLE.\n[!] ERROR: %s\n[!] ALL LOGS WILL BE REDIRECTED TO STDERR (2).\n", strerror(errno));
		zlog_init_stderr();
		return;
	} else {

		// +1 because of trailing null byte
		int log_folder_dir_length = strlen(home) + strlen (LIBRARY_FOLDER) + 1;
		char log_folder_dir[log_folder_dir_length];
		snprintf(log_folder_dir, log_folder_dir_length, "%s%s", home, LIBRARY_FOLDER);

		DIR *folder_dir = opendir(log_folder_dir);
		if(folder_dir){
			closedir(folder_dir);
		} else {
			if(mkdir_wrapper(log_folder_dir, 0755) == -1){ // (Bear in mind umask)
				fprintf(stderr, "[!] ERROR CREATING LOG DIRECTORY.\n[!] ERROR: %s\n[!] ALL LOGS WILL BE REDIRECTED TO STDERR (2).\n", strerror(errno));
				zlog_init_stderr();
				return;
			}
			closedir(folder_dir);
		}

		char *program_name = GET_PROGRAM_NAME();
		// +3 because of "/" and null trailling byte
		int log_dir_length = strlen(home) + strlen (LIBRARY_FOLDER) + strlen(program_name) + 3;
		char log_dir[log_dir_length];

		snprintf(log_dir, log_dir_length, "%s%s/%s", home, LIBRARY_FOLDER, program_name);
		
		DIR *dir = opendir(log_dir);
		if(dir){ //Directory exists
			closedir(dir);
			create_log_file_and_start_logger(log_dir);
		} else if(errno == ENOENT){ //Directory does not exist; create
			if(mkdir_wrapper(log_dir, 0755) == -1){ // (Bear in mind umask)
				fprintf(stderr, "[!] ERROR CREATING LOG DIRECTORY.\n[!] ERROR: %s\n[!] ALL LOGS WILL BE REDIRECTED TO STDERR (2).\n", strerror(errno));
				zlog_init_stderr();
				return;
			}
			closedir(dir);
			create_log_file_and_start_logger(log_dir);
		} else {
			fprintf(stderr, "[!] ERROR: %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}
	}

	printf("I GET OUT OF %s\n", __func__);
	
}


void create_log_file_and_start_logger(const char *log_dir){

	printf("I REACH %s\n", __func__);

	time_t ltime;
	ltime = time(NULL);
	char char_pid[10];
	char date_and_time[80];

	struct tm *tm_struct = localtime(&ltime);

	sprintf(char_pid, "%d", getpid());
	strftime(date_and_time, sizeof(date_and_time), "%Y-%m-%d_%H:%M:%S", tm_struct);
	
	//+2 because  of "/" and because strlen excludes the terminating null byte
	int program_name_length = strlen(char_pid) + strlen(date_and_time) + strlen(".log") + 2;
	
	// +1 because of "_"
	int log_file_absolute_path_length = strlen(log_dir) + program_name_length + 1;

	char log_file_absolute_path[log_file_absolute_path_length]; 
	snprintf(log_file_absolute_path, log_file_absolute_path_length, "%s/%s_%s.log", log_dir, char_pid, date_and_time);

	if ( original_open == NULL ) {
		original_open = dlsym_wrapper("open");
	}

	if(original_open(log_file_absolute_path, O_CREAT | O_EXCL, 0644) == -1){
		fprintf(stderr, "[!] ERROR CREATING LOG FILE.\n[!] ERROR: %s\n[!] ALL LOGS WILL BE REDIRECTED TO STDERR (2).\n", strerror(errno));
		zlog_init_stderr();
	} else {
		zlog_init(log_file_absolute_path);
	}

	printf("I GET OUT OF %s\n", __func__);

}

void create_temp_dir(){

	printf("I REACH %s\n", __func__);

	bool temp_folder_exists;
	char *temp_dir = getenv("HOME");
	char char_pid[10];
	if(!temp_dir){
		fprintf(stderr, "[!] ERROR RETRIEVING HOME ENV VARIABLE.\n[!] ERROR: %s\n[!] PLEASE CREATE IT.\n", strerror(errno));
		temp_dir = get_current_dir_name();
	}

	sprintf(char_pid, "%d", getpid());

	time_t ltime;
	ltime = time(NULL);
	char date_and_time[80];

	struct tm *tm_struct = localtime(&ltime);
	strftime(date_and_time, sizeof(date_and_time), "%Y-%m-%d_%H:%M:%S", tm_struct);

	// +1 becuase of trailing null byte 
	int total_tmp_dir_length = strlen(temp_dir) + strlen(LIBRARY_FOLDER) + strlen(TEMPORAL_LINKS_FOLDER) + 1;

	char temp_dir_aux[total_tmp_dir_length];
	snprintf(temp_dir_aux, total_tmp_dir_length, "%s%s%s", temp_dir, LIBRARY_FOLDER, TEMPORAL_LINKS_FOLDER);

	DIR *aux_dir = opendir(temp_dir_aux);
	if(aux_dir){
		// Directory already exists
		temp_folder_exists = true;
		closedir(aux_dir);
	} else if(errno == ENOENT){ 
		// Directory does not exist; Create it
		if(mkdir_wrapper(temp_dir_aux, 0755) == -1){ // (Bear in mind umask)
			fprintf(stderr, "[!] ERROR CREATING TMP 1st DIRECTORY.\n[!] ERROR: %s\n", strerror(errno));	
		} else {
			printf("Le fichier is creationne\n");
			temp_folder_exists = true;
		}
	} else {
		fprintf(stderr, "[!] ERROR CREATING TMP 1st DIRECTORY.\n[!] ERROR: %s\n[!] ABORTING\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	// +3 becuase of "/", "_" and trailing null byte
	total_tmp_dir_length = strlen(GET_PROGRAM_NAME()) + strlen(char_pid) + strlen(date_and_time) + 3;
	char temp_dir_aux2[total_tmp_dir_length];
	snprintf(temp_dir_aux2, total_tmp_dir_length, "/%s_%s%s", GET_PROGRAM_NAME(), char_pid, date_and_time);
	char *final_temp_dir;
	if(temp_folder_exists) {
		// Safe concatenation of strings
		if(asprintf(&final_temp_dir, "%s%s", temp_dir_aux, temp_dir_aux2) == -1){
			fprintf(stderr, "[!] ERROR saving memory for temporal directory name construction (concatenation).\n[!] ERROR: %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}

	} else {
		// Safe concatenation of strings
		if(asprintf(&final_temp_dir, "%s%s", temp_dir, temp_dir_aux2) == -1){
			fprintf(stderr, "[!] ERROR saving memory for temporal directory name construction (concatenation).\n[!] ERROR: %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}
	}

	printf("Trying to create: %s\n", final_temp_dir);

	if(mkdir_wrapper(final_temp_dir, 0755) == -1){ // (Bear in mind umask)
		fprintf(stderr, "[!] ERROR CREATING TMP 2nd DIRECTORY.\n[!] ERROR: %s\n", strerror(errno));
	} else {
		temp_dir = final_temp_dir;
	}

	// If th path doesn't end with "/", add it.
	if(temp_dir[strlen(temp_dir)-1] != '/'){
		// Safe concatenation of strings
		if(asprintf(&temp_dir, "%s%s", temp_dir, "/") == -1){
			fprintf(stderr, "[!] ERROR saving memory for temporal directory name construction (concatenation).\n[!] ERROR: %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}

	}

	printf("EL DIRECTORIO QUE SE HA CALCULADO ES: %s\n", temp_dir);

	g_temp_dir = strdup(temp_dir);

	free(temp_dir);
	free(final_temp_dir);

	printf("I GET OUT OF %s\n", __func__);

}

/// ########## Logger ##########

/// <-------------------------------------------------> 

/// ########## Logic ##########

/*
    Checks properties of the given parameters, this is, the given path and 
    inode. Checking properties in this context means checking if a 
    file_object_info with the same path already exists in the array. If it
    doesn't, insert it and return true, othwerwise (if it does) compare 
    the given inode and the inode of the file_object_info. If they're equal
    return true, otherwise return false (TOCTTOU detected). 
*/
void check_parameters_properties(const char *path, const char *caller_function_name){

	if(file_does_exist(path)){
		ino_t inode = get_inode(path);

		int index = find_index_in_array(&g_array, path);
		
		if(index < 0){
			upsert_inode_in_array(&g_array, path, inode, g_temp_dir);
		} else {
			file_object_info aux = get_from_array_at_index(&g_array,index);
			if(aux.inode != inode){
			//printf("FILE %s EXISTS: %d\n", path, exists);
				zlogf_time(ZLOG_INFO_LOG_MSG, "[+][!] WARNING! TOCTTOU DETECTED! [+][!]\n Inode of <%s> has changed since it was previously invoked. Threat detected when invoking <%s> function. Inode was <%lu> and now it is <%lu>. \n [#] PROGRAM %s ABORTED [#]\n\n", path, caller_function_name, aux.inode, inode, GET_PROGRAM_NAME());
				fprintf(stderr,"[+][!] WARNING! TOCTTOU DETECTED!. [!][+]\n[#] PROGRAM %s ABORTED [#]\n[#] Check logs for more info [#]\n[!] LOGIFLE: %s [!]\n", GET_PROGRAM_NAME(), zlog_get_log_file_name());
				fflush(stdout);
				zlog_flush_buffer();
				exit(EXIT_FAILURE);
			} else {
				// #### EMPTY BLOCK TODO!
				//printf("NODES ARE EQUAL!!! :) HAPPINESS");d
			}
		}
	} else { // if file_does_exist
		zlogf_time(ZLOG_DEBUG_LOG_MSG, "Function %s called with path %s that does not exist. Inserting in array with negative %d inode.\n", caller_function_name, path, NONEXISTING_FILE_INODE);
		upsert_inode_in_array(&g_array, path, NONEXISTING_FILE_INODE, NULL);
	}
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

	if (fchdir(directory_fd) == -1){
		zlogf_time(ZLOG_DEBUG_LOG_MSG, "[!] FCHDIR ERROR: %s\n", strerror(errno));
	}

	const char *res = sanitize_and_get_absolute_path(src);

    // Restoring working dir
    chdir_wrapper(original_working_dir);
	free (original_working_dir);

    return res;
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
	Function used to retrieve as string the full directory path pointed to
	by a given file descriptor. 
*/
char * get_directory_from_fd(int directory_fd){
	char *original_working_dir = get_current_dir_name();

	if (fchdir(directory_fd) == -1){
		zlogf_time(ZLOG_DEBUG_LOG_MSG, "[!] FCHDIR ERROR: %s\n", strerror(errno));
	}

	char *directory_fd_path = get_current_dir_name();

	chdir_wrapper(original_working_dir);

	free (original_working_dir);

	return directory_fd_path;
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

/// ########## Logic ##########

/// <-------------------------------------------------> 

/// ########## Core and useful functions ##########





/// ########## Core and useful functions ##########

/// <-------------------------------------------------> 

/// ########## Hooked functions replacement code ##########

int __xstat(int ver, const char *path, struct stat *buf)
{	

	printf("I REACH %s with params %s\n", __func__, path);

	if(LIBRARY_ON){
		const char *sanitized_path = sanitize_and_get_absolute_path(path);

		print_function_and_path(__func__, path, sanitized_path);

		upsert_inode_in_array(&g_array, sanitized_path, get_inode(sanitized_path), g_temp_dir);

		printf("I REACH THE MIDDLE OF %s\n", __func__);
	}
	if ( original_xstat == NULL ) {
		original_xstat = dlsym_wrapper(__func__);
	}

	printf("I GET OUT OF %s\n", __func__);

	return original_xstat(ver, path, buf);
} 

int __xstat64(int ver, const char *path, struct stat64 *buf)
{

	if(LIBRARY_ON){
		const char *sanitized_path = sanitize_and_get_absolute_path(path);

		print_function_and_path(__func__, path,sanitized_path);

		upsert_inode_in_array(&g_array, sanitized_path, get_inode(sanitized_path), g_temp_dir);
	}

	if ( original_xstat64 == NULL ) {
		original_xstat64 = dlsym_wrapper(__func__);
	}
	return original_xstat64(ver, path, buf);
}

int __lxstat(int ver, const char *path, struct stat *buf)
{
	
	if(LIBRARY_ON){
		const char *sanitized_path = sanitize_and_get_absolute_path(path);

		print_function_and_path(__func__, path, sanitized_path);

		upsert_inode_in_array(&g_array, sanitized_path, get_inode(sanitized_path), g_temp_dir);
	}	

	if ( original_lxstat == NULL ) {
		original_lxstat = dlsym_wrapper(__func__);
	}

	return original_lxstat(ver,path, buf);
}

int __lxstat64(int ver, const char *path, struct stat64 *buf)
{
	if(LIBRARY_ON){
		const char *sanitized_path = sanitize_and_get_absolute_path(path);

		print_function_and_path(__func__, path, sanitized_path);

		upsert_inode_in_array(&g_array, sanitized_path, get_inode(sanitized_path), g_temp_dir);
	}

	if ( original_lxstat64 == NULL ) {
		original_lxstat64 = dlsym_wrapper(__func__);
	}

	return original_lxstat64(ver,path, buf);
}

/*
	Since open can also create files, we hook it and check whether a new file
	has been created. In order to do so, we check if the file existed before
	the actual call. If it hasn't, then we check if, after the call, the file
	has been indeed created (via fstat). If it has, we simply update its 
	corresponding array entry or insert it.
*/
int open(const char *path, int flags, ...)
{

		printf("I REACH %s\n", __func__);

	int open_result;
	if(LIBRARY_ON){
		const char *sanitized_path = sanitize_and_get_absolute_path(path);

		print_function_and_path(__func__, path, sanitized_path);

		bool path_exists_before = file_does_exist(sanitized_path);
		struct stat new_file;

		check_parameters_properties(sanitized_path, __func__);

		va_list variable_arguments;
		va_start(variable_arguments, flags);

		open_result = open_wrapper(path, flags, variable_arguments);

		va_end(variable_arguments);

	/*
		open(), openat(), and creat() return the new file descriptor, or -1
		if an error occurred (in which case, errno is set appropriately).
	*/

		if(open_result == -1){
			zlogf_time(ZLOG_INFO_LOG_MSG, "[!] OPEN ERROR: %s\n", strerror(errno));
		} else {
		/*
		If file didn't exist before actual open call and fstat returns
		zero (success), a new file has been created.
		*/
			if(!path_exists_before && !fstat(open_result, &new_file)){
			/*
				New file has been just created. Now there are two options:
					- There is already an entry in the array referencing the path
						so only the inode must be updated.
					- There is no entry in the array so just insert.
				Both actions are carreid out by upsert_inode_in_array
			*/

				ino_t inode = get_inode(sanitized_path);

				upsert_inode_in_array(&g_array, sanitized_path, inode, g_temp_dir);

			}

		}
	} else {

		va_list variable_arguments;
		va_start(variable_arguments, flags);

		open_result = open_wrapper(path, flags, variable_arguments);

		va_end(variable_arguments);
	}

	printf("I GET OUT OF %s\n", __func__);

	return open_result;
}

/*
	open64() behaves exactly in the same way as open(). Please
	read open() docs.
*/
int open64(const char *path, int flags, ...){

	int open64_result;
	
	if(LIBRARY_ON){

		const char *sanitized_path = sanitize_and_get_absolute_path(path);

		print_function_and_path(__func__, path, sanitized_path);

		bool path_exists_before = file_does_exist(sanitized_path);
		struct stat new_file;

		check_parameters_properties(sanitized_path, __func__);

		va_list variable_arguments;
		va_start(variable_arguments, flags);

		open64_result = open64_wrapper(path, flags, variable_arguments);

		va_end(variable_arguments);

	/*
		open(), openat(), and creat() return the new file descriptor, or -1
		if an error occurred (in which case, errno is set appropriately).
	*/

		if(open64_result == -1){
			zlogf_time(ZLOG_DEBUG_LOG_MSG, "[+] OPEN64 ERROR: %s\n", strerror(errno));
		} else {

			if(!path_exists_before && !fstat(open64_result, &new_file)){

				ino_t inode = get_inode(sanitized_path);

				upsert_inode_in_array(&g_array, sanitized_path, inode, g_temp_dir);

			}

		}
	} else {
		va_list variable_arguments;
		va_start(variable_arguments, flags);

		open64_result = open64_wrapper(path, flags, variable_arguments);

		va_end(variable_arguments);
	}
	return open64_result;
}

int access(const char *path, int mode){

	if(LIBRARY_ON){

		const char *sanitized_path = sanitize_and_get_absolute_path(path);

		print_function_and_path(__func__, path, sanitized_path);

		upsert_inode_in_array(&g_array, sanitized_path, get_inode(sanitized_path), g_temp_dir);
	}
	if(original_access == NULL){
		original_access = dlsym_wrapper(__func__);
	}	

	return original_access(path, mode);

}

FILE *fopen(const char *path, const char *mode){

	if(original_fopen == NULL){
		original_fopen = dlsym_wrapper(__func__);
	}
	FILE *fopen_result;

	if(LIBRARY_ON){
		const char *sanitized_path = sanitize_and_get_absolute_path(path);

		print_function_and_path(__func__, path, sanitized_path);

		check_parameters_properties(sanitized_path, __func__);

		bool path_exists_before = file_does_exist(sanitized_path);
		struct stat new_file;

		fopen_result = fopen_wrapper(path, mode);

		if(fopen_result == NULL){
			zlogf_time(ZLOG_INFO_LOG_MSG, "[!] FOPEN ERROR: %s\n", strerror(errno));
		} else {
			if(!path_exists_before && !fstat(fileno(fopen_result), &new_file)){
			// A new file has been created.
				upsert_inode_in_array(&g_array, sanitized_path, get_inode(sanitized_path), g_temp_dir);
			}
		}
	} else {
		fopen_result = fopen_wrapper(path, mode);
	}

	return fopen_result;

}

int unlink(const char *path){
	if(original_unlink == NULL){
		original_unlink = dlsym_wrapper(__func__);
	}
	int unlink_result;
	if(LIBRARY_ON){

		const char *sanitized_path = sanitize_and_get_absolute_path(path);

		print_function_and_path(__func__, path, sanitized_path);

		check_parameters_properties(sanitized_path, __func__);

		unlink_result = original_unlink(path);

	/*
		On success, zero is returned. On error, -1 is returned, and errno is
		set appropriately.
	*/
		if(unlink_result == -1){
			zlogf_time(ZLOG_INFO_LOG_MSG, "[!] UNLINK ERROR: %s\n", strerror(errno));
		} else {
			int index = find_index_in_array(&g_array, sanitized_path);
			if(index >= 0){
				remove_from_array_at_index(&g_array, index);
			}
		}
	} else {
		unlink_result = original_unlink(path);
	}

	return unlink_result;

}

int unlinkat(int dirfd, const char *path, int flags){

	if(original_unlinkat == NULL){
		original_unlinkat = dlsym_wrapper(__func__);
	}
	int unlinkat_result;

	if(LIBRARY_ON){
		
		const char *sanitized_path;

		if(path_is_absolute(path)){
			sanitized_path = sanitize_and_get_absolute_path(path);
		} else {
			sanitized_path = sanitize_and_get_absolute_path_from_dir_file_descriptor(path, dirfd);
		}

		print_function_and_path(__func__, path, sanitized_path);

		check_parameters_properties(sanitized_path, __func__);

		

	/*
		Note that the original function gets passed only path, not sanitized_path
		since the retrieval of sanitized_path is a mere operation in order to
		keep g_array updated.
	*/
		unlinkat_result = original_unlinkat(dirfd, path, flags);

   	/*
		On success, unlinkat() returns 0. On error, -1 is returned and errno
		is set to indicate the error.
   	*/
		if(unlinkat_result == -1){
			zlogf_time(ZLOG_INFO_LOG_MSG, "[!] UNLINKAT ERROR: %s\n", strerror(errno));
		} else {

			int index = find_index_in_array(&g_array, sanitized_path);

			if(index >= 0){
				remove_from_array_at_index(&g_array, index);
			}
		}
	} else {
		unlinkat_result = original_unlinkat(dirfd, path, flags);
	}

	return unlinkat_result;

}

int openat(int dirfd, const char *path, int flags, ...){

	int openat_result;

	if(LIBRARY_ON){

		const char *sanitized_path;

		if(path_is_absolute(path)){
			sanitized_path = sanitize_and_get_absolute_path(path);
		} else {
			sanitized_path = sanitize_and_get_absolute_path_from_dir_file_descriptor(path, dirfd);
		}

		print_function_and_path(__func__, path, sanitized_path);

		bool path_exists_before = file_does_exist(sanitized_path);
		struct stat new_file;

		check_parameters_properties(sanitized_path, __func__);

		va_list variable_arguments;
		va_start(variable_arguments, flags);

		openat_result =  openat_wrapper(dirfd, path, flags, variable_arguments);

		va_end(variable_arguments);

	/*
		On success, openat() returns a new file descriptor. On error, -1 is
		returned and errno is set to indicate the error.
	*/
		if(openat_result == -1){
			zlogf_time(ZLOG_INFO_LOG_MSG, "[!] OPENAT ERROR: %s\n", strerror(errno));
		} else {

			if(!path_exists_before && !fstat(openat_result, &new_file)){

				ino_t inode = get_inode(sanitized_path);
				
				upsert_inode_in_array(&g_array, sanitized_path, inode, g_temp_dir);
				

			}

		}
	} else{
		va_list variable_arguments;
		va_start(variable_arguments, flags);
		openat_result =  openat_wrapper(dirfd, path, flags, variable_arguments);
		va_end(variable_arguments);
	}

	return openat_result;

}


/*
	Creates a symbolic link called newpath that poins to oldpath.
*/
int symlink(const char *oldpath, const char *newpath){

	int symlink_result;

	if(LIBRARY_ON){
		
		const char *sanitized_newpath = sanitize_and_get_absolute_path(newpath);

		print_function_and_path(__func__, newpath, sanitized_newpath);

		check_parameters_properties(sanitized_newpath, __func__);
		
		

		symlink_result = symlink_wrapper(oldpath, newpath);

    /*
		Upon successful completion, symlink() shall return 0; otherwise, it 
		shall return -1 and set errno to indicate the error.
    */
		if(symlink_result == -1){
			zlogf_time(ZLOG_INFO_LOG_MSG, "[!] SYMLINK ERROR: %s\n", strerror(errno));
		} else {

			ino_t inode = get_inode(sanitized_newpath);

			upsert_inode_in_array(&g_array, sanitized_newpath, inode, g_temp_dir);
			
		}
	}
	else{
		symlink_result = symlink_wrapper(oldpath, newpath);
	}

	return symlink_result;

}

/*
	Creates a symbolic link to oldpath called newpath in the directory pointed to by newdirfd.
*/
int symlinkat(const char *oldpath, int newdirfd, const char *newpath){


	if(original_symlinkat == NULL){
		original_symlinkat = dlsym_wrapper(__func__);
	}
	int symlinkat_result;

	if(LIBRARY_ON){


		const char *sanitized_new_path;

		if(path_is_absolute(newpath)){
			sanitized_new_path = sanitize_and_get_absolute_path(newpath);
		} else {
			sanitized_new_path = sanitize_and_get_absolute_path_from_dir_file_descriptor(newpath, newdirfd);
		}

		print_function_and_path(__func__, newpath, sanitized_new_path);

		check_parameters_properties(sanitized_new_path, __func__);
		


		symlinkat_result = original_symlinkat(oldpath, newdirfd, newpath);

	/*
		On success, symlinkat() returns 0. On error, -1 is returned and errno
		is set to indicate the error.
	*/
		if(symlinkat_result == -1){
			zlogf_time(ZLOG_INFO_LOG_MSG, "[!] SYMLINKAT ERROR: %s\n", strerror(errno));
		} else {
			ino_t inode = get_inode(sanitized_new_path);
			upsert_inode_in_array(&g_array, sanitized_new_path, inode, g_temp_dir);
		}
	} else {
		symlinkat_result = original_symlinkat(oldpath, newdirfd, newpath);
	}

	return symlinkat_result;

}

/*
	remove() deletes a name from the file system. 
	It calls unlink(2) for files, and rmdir(2) for directories.	
*/
int remove(const char *path) {

	if(original_remove == NULL){
		original_remove = dlsym_wrapper(__func__);
	}

	int remove_result;

	if(LIBRARY_ON){

		const char *sanitized_path = sanitize_and_get_absolute_path(path);

		print_function_and_path(__func__, path, sanitized_path);

		check_parameters_properties(sanitized_path, __func__);



		remove_result = original_remove(path);

	/*
		On success, zero is returned. On error, -1 is returned, and errno is
		set appropriately.
	*/
		if(remove_result == -1){
			zlogf_time(ZLOG_INFO_LOG_MSG, "[!] REMOVE ERROR: %s\n", strerror(errno));
		} else {
			int index = find_index_in_array(&g_array, sanitized_path);

			if(index >= 0){
				remove_from_array_at_index(&g_array, index);
			}
		}
	} else {
		remove_result = original_remove(path);
	}

	return remove_result;

}

/*
	The system call mknod() creates a filesystem node (file, device
    special file, or named pipe) named pathname, with attributes
    specified by mode and dev.
*/
int __xmknod(int ver, const char *path, mode_t mode, dev_t *dev){

	if(original_xmknod == NULL){
		original_xmknod = dlsym_wrapper(__func__);
	}

	int mknod_result;

	if(LIBRARY_ON){

		const char *sanitized_path = sanitize_and_get_absolute_path(path);

		print_function_and_path(__func__, path, sanitized_path);

		check_parameters_properties(sanitized_path, __func__);



		mknod_result = original_xmknod(ver, path, mode, dev);

    /*
		mknod() and mknodat() return zero on success, or -1 if an error
		occurred (in which case, errno is set appropriately).
    */
		if(mknod_result == -1){
			zlogf_time(ZLOG_INFO_LOG_MSG, "[!] MKNOD ERROR: %s\n", strerror(errno));
		} else {

			ino_t inode = get_inode(sanitized_path);

			upsert_inode_in_array(&g_array, sanitized_path, inode, g_temp_dir);

		}
	} else {
		mknod_result = original_xmknod(ver, path, mode, dev);
	}

	return mknod_result;

}

/*
	The mknodat() system call operates in exactly the same way as mknod(2), 
	except that it creates a speial or ordinary file relative to a 
	directory file descriptor.
*/
int __xmknodat(int ver, int dirfd, const char *path, mode_t mode, dev_t *dev){


	if(original_xmknodat == NULL){
		original_xmknodat = dlsym_wrapper(__func__);
	}

	int mknodat_result;

	if(LIBRARY_ON){

		const char *sanitized_path;

		if(path_is_absolute(path)){
			sanitized_path = sanitize_and_get_absolute_path(path);
		} else {
			sanitized_path = sanitize_and_get_absolute_path_from_dir_file_descriptor(path, dirfd);
		}

		print_function_and_path(__func__, path, sanitized_path);

		check_parameters_properties(sanitized_path, __func__);


		mknodat_result = original_xmknodat(ver, dirfd, path, mode, dev);

	/*
		On success, mknodat() returns 0. On error, -1 is returned and errno is
		set to indicate the error.
	*/
		if(mknodat_result == -1){ 
			zlogf_time(ZLOG_INFO_LOG_MSG, "[!] MKNODAT ERROR: %s\n", strerror(errno));
		} else {
			ino_t inode = get_inode(sanitized_path);
			upsert_inode_in_array(&g_array, sanitized_path, inode, g_temp_dir);
		}

	} else {
		mknodat_result = original_xmknodat(ver, dirfd, path, mode, dev);
	}

	return mknodat_result;
}


/*
	Creates a new hardlink called newpath that points to oldpath. 
*/
int link(const char *oldpath, const char *newpath){

	int link_result;

	if(LIBRARY_ON){

		const char *sanitized_new_path = sanitize_and_get_absolute_path(newpath);

		print_function_and_path(__func__, newpath, sanitized_new_path);

		check_parameters_properties(sanitized_new_path, __func__);



		link_result = link_wrapper(oldpath, newpath);

   	/*
		Upon successful completion, 0 shall be returned. Otherwise, -1 shall
		be returned and errno set to indicate the error.
   	*/
		if(link_result == -1){
			zlogf_time(ZLOG_INFO_LOG_MSG, "[!] LINK ERROR: %s\n", strerror(errno));
		} else {

			ino_t inode = get_inode(sanitized_new_path);

			upsert_inode_in_array(&g_array, sanitized_new_path, inode, g_temp_dir);

		}

	} else {
		link_result = link_wrapper(oldpath, newpath);
	}

	return link_result;

}


/*
	Creates a new hardlink called newpath in directory pointed to by newdirfd. The hardlink newpath
	points to file oldpath, which is located in directroy pointed to by olddirfd. 
*/
int linkat(int olddirfd, const  char *oldpath, int newdirfd, const char *newpath, int flags){


	if(original_linkat == NULL){
		original_linkat = dlsym_wrapper(__func__);
	}

	int linkat_result;

	if(LIBRARY_ON){

		const char *full_new_path;

		if(path_is_absolute(newpath)){
			full_new_path = sanitize_and_get_absolute_path(newpath);
		} else {
			full_new_path = sanitize_and_get_absolute_path_from_dir_file_descriptor(newpath, newdirfd);
		}

		print_function_and_path(__func__, newpath, full_new_path);

		check_parameters_properties(full_new_path, __func__);



		linkat_result = original_linkat(olddirfd, oldpath, newdirfd, newpath, flags);

    /*
		On success, linkat() returns 0. On error, -1 is returned and errno is
		set to indicate the error.
    */
		if(linkat_result == -1){
			zlogf_time(ZLOG_INFO_LOG_MSG, "[!] LINKAT ERROR: %s\n", strerror(errno));
		} else {

			ino_t inode = get_inode(full_new_path);
			upsert_inode_in_array(&g_array, full_new_path, inode, g_temp_dir);

		}
	} else {
		linkat_result = original_linkat(olddirfd, oldpath, newdirfd, newpath, flags);
	}

	return linkat_result;

}

int creat64(const char *path, mode_t mode){

	if(original_creat64 == NULL){
		original_creat64 = dlsym_wrapper(__func__);
	}

	int creat64_result;

	if(LIBRARY_ON){

		const char *sanitized_path = sanitize_and_get_absolute_path(path);

		print_function_and_path(__func__, path, sanitized_path);

		check_parameters_properties(sanitized_path, __func__);



		creat64_result = original_creat64(path, mode);

    /*
		If successful, creat() and creat64() return a nonnegative integer, 
		which is the lowest numbered unused valid file descriptor. On failure,
		they return -1, do not create or modify any files, and set errno to 
		one of the following values:
    */
		if(creat64_result == -1){
			zlogf_time(ZLOG_INFO_LOG_MSG, "[!] CREAT64 ERROR: %s\n", strerror(errno));
		} else {
			upsert_inode_in_array(&g_array, sanitized_path, get_inode(sanitized_path), g_temp_dir);
		}
	} else {
		creat64_result = original_creat64(path, mode);
	}

	return creat64_result;

}

int creat(const char *path, mode_t mode){

	if(original_creat == NULL){
		original_creat = dlsym_wrapper(__func__);
	}

	int creat_result;

	if(LIBRARY_ON){

		const char *sanitized_path = sanitize_and_get_absolute_path(path);

		print_function_and_path(__func__, path, sanitized_path);

		check_parameters_properties(sanitized_path, __func__);



		creat_result = original_creat(path, mode);

     /*
		If successful, creat() and creat64() return a nonnegative integer, 
		which is the lowest numbered unused valid file descriptor. On failure,
		they return -1, do not create or modify any files, and set errno to 
		one of the following values:
    */
		if(creat_result == -1){
			zlogf_time(ZLOG_INFO_LOG_MSG, "[!] CREAT ERROR: %s\n", strerror(errno));
		} else {
			upsert_inode_in_array(&g_array, sanitized_path, get_inode(sanitized_path), g_temp_dir);
		}
	} else {
		creat_result = original_creat(path, mode);
	}

	return creat_result;


}

int rmdir(const char *path){

	if(original_rmdir == NULL){
		original_rmdir = dlsym_wrapper(__func__);
	}


	int rmdir_result;

	if(LIBRARY_ON){

		const char *sanitized_path = sanitize_and_get_absolute_path(path);

		print_function_and_path(__func__, path, sanitized_path);

		check_parameters_properties(sanitized_path, __func__);


		rmdir_result = original_rmdir(path);

	/*
		Upon successful completion, the function rmdir() shall return 0. 
		Otherwise, -1 shall be returned, and errno set to indicate the error. 
		If -1 is returned, the named directory shall not be changed.
	*/
		if(rmdir_result == -1){
			zlogf_time(ZLOG_INFO_LOG_MSG, "[!] RMDIR ERROR: %s\n", strerror(errno));
		} else {
			int index = find_index_in_array(&g_array, sanitized_path);

			if(index >= 0){
				remove_from_array_at_index(&g_array, index);
			//g_array.list[index].inode = -1;
			}
		}
	} else {
		rmdir_result = original_rmdir(path);
	}

	return rmdir_result;

}

ssize_t readlink(const char *pathname, char *buf, size_t bufsiz){

	if ( original_readlink == NULL ) {
		original_readlink = dlsym_wrapper(__func__);
	}

	int readlink_result;
	if(LIBRARY_ON){

		const char* sanitized_pathname = sanitize_and_get_absolute_path(pathname);

		print_function_and_path(__func__, pathname, sanitized_pathname);



		readlink_result = original_readlink(pathname, buf, bufsiz);

		upsert_inode_in_array(&g_array, sanitized_pathname, get_inode(sanitized_pathname), g_temp_dir);

		if(readlink_result == -1){
			zlogf_time(ZLOG_INFO_LOG_MSG, "[!] READLINK ERROR: %s\n", strerror(errno));
		} else {

			ino_t inode = get_inode(sanitized_pathname);
			upsert_inode_in_array(&g_array, sanitized_pathname, inode, g_temp_dir);

		}
	} else {
		readlink_result = original_readlink(pathname, buf, bufsiz);
	}

	return readlink_result;
}

ssize_t readlinkat(int dirfd, const char *pathname, char *buf, size_t bufsiz){


	if(original_readlinkat == NULL){
		original_readlinkat = dlsym_wrapper(__func__);
	}

	int readlinkat_result;

	if(LIBRARY_ON){

		const char *sanitized_path;
		if(path_is_absolute(pathname)){
			sanitized_path = sanitize_and_get_absolute_path(pathname);
		} else {
			sanitized_path = sanitize_and_get_absolute_path_from_dir_file_descriptor(pathname, dirfd);
		}

		print_function_and_path(__func__, pathname, sanitized_path);

		readlinkat_result = original_readlinkat(dirfd, pathname, buf, bufsiz);

    /*
		On success, these calls return the number of bytes placed in buf.
       	(If the returned value equals bufsiz, then truncation may have
       	occurred.)  On error, -1 is returned and errno is set to indicate the
       	error.
    */
		if(readlinkat_result == -1){
			zlogf_time(ZLOG_INFO_LOG_MSG, "[!] READLINKAT ERROR: %s\n", strerror(errno));
		} else {

			ino_t inode = get_inode(sanitized_path);
			upsert_inode_in_array(&g_array, sanitized_path, inode, g_temp_dir);

		}
	} else {
		readlinkat_result = original_readlinkat(dirfd, pathname, buf, bufsiz);
	}

	return readlinkat_result;
}

int rename(const char *oldpath, const char *newpath){


	if(original_rename == NULL){
		original_rename = dlsym_wrapper(__func__);
	}

	int rename_result;
	if(LIBRARY_ON){

		const char *sanitized_old_path = sanitize_and_get_absolute_path(oldpath);
		const char *sanitized_new_path = sanitize_and_get_absolute_path(newpath);

		print_function_and_path(__func__, newpath, sanitized_new_path);

		check_parameters_properties(sanitized_new_path, __func__);


		rename_result = original_rename(oldpath, newpath);

	/*
		On success, zero is returned.  On error, -1 is returned, and errno is
    	set appropriately.
	*/

		if(rename_result == -1){
			zlogf_time(ZLOG_INFO_LOG_MSG, "[!] RENAME ERROR: %s\n", strerror(errno));
		} else {

			int index = find_index_in_array(&g_array, sanitized_old_path);
			if(index > -1){
				remove_from_array_at_index(&g_array, index);
			}

			ino_t inode = get_inode(sanitized_new_path);
			upsert_inode_in_array(&g_array, newpath, inode, g_temp_dir);

		}
	} else {
		rename_result = original_rename(oldpath, newpath);
	}

	return rename_result;
}

int renameat(int olddirfd, const char *oldpath, int newdirfd, const char *newpath){

	if(original_renameat == NULL){
		original_renameat = dlsym_wrapper(__func__);
	}

	int renameat_result;
	if(LIBRARY_ON){

		const char *sanitized_new_path;
		const char *sanitized_old_path;

		if(path_is_absolute(newpath)){
			sanitized_new_path = sanitize_and_get_absolute_path(newpath);
		} else {
			sanitized_new_path = sanitize_and_get_absolute_path_from_dir_file_descriptor(newpath, newdirfd);
		}

		if(path_is_absolute(oldpath)){
			sanitized_old_path = sanitize_and_get_absolute_path(oldpath);
		} else {
			sanitized_old_path = sanitize_and_get_absolute_path_from_dir_file_descriptor(oldpath, newdirfd);
		}

		print_function_and_path(__func__, newpath, sanitized_new_path);

		check_parameters_properties(sanitized_new_path, __func__);



		renameat_result = original_renameat(olddirfd, oldpath, newdirfd, newpath);

	/*
		On success, zero is returned.  On error, -1 is returned, and errno is
    	set appropriately.
	*/

		if(renameat_result == -1){
			zlogf_time(ZLOG_INFO_LOG_MSG, "[!] RENAME ERROR: %s\n", strerror(errno));
		} else {

			int index = find_index_in_array(&g_array, sanitized_old_path);
			if(index > -1){
				remove_from_array_at_index(&g_array, index);
			}

			ino_t inode = get_inode(sanitized_new_path);
			upsert_inode_in_array(&g_array, newpath, inode, g_temp_dir);

		}
	} else {
		renameat_result = original_renameat(olddirfd, oldpath, newdirfd, newpath);
	}

	return renameat_result;
}

FILE *fopen64(const char *path, const char *mode){

	if(original_fopen64 == NULL){
		original_fopen64 = dlsym_wrapper(__func__);
	} 

	FILE *fopen64_result;

	if(LIBRARY_ON){

		const char *sanitized_path = sanitize_and_get_absolute_path(path);

		print_function_and_path(__func__, path, sanitized_path);

		check_parameters_properties(sanitized_path, __func__);

		bool path_exists_before = file_does_exist(sanitized_path);
		struct stat new_file;



		fopen64_result = original_fopen64(path, mode);

		if(fopen64_result == NULL){
			zlogf_time(ZLOG_INFO_LOG_MSG, "[!] FOPEN64 ERROR: %s\n", strerror(errno));
		} else {
			if(!path_exists_before && !fstat(fileno(fopen64_result), &new_file)){
				upsert_inode_in_array(&g_array, sanitized_path, get_inode(sanitized_path), g_temp_dir);
			}
		}
	} else {
		fopen64_result = original_fopen64(path, mode);
	}

	return fopen64_result;

}

FILE *freopen(const char *pathname, const char *mode, FILE *stream){

	if(original_freopen == NULL){
		original_freopen = dlsym_wrapper(__func__);
	}

	FILE *freopen_result;

	if(LIBRARY_ON){

		const char *sanitized_pathname = sanitize_and_get_absolute_path(pathname);

		print_function_and_path(__func__, pathname, sanitized_pathname);

		check_parameters_properties(sanitized_pathname, __func__);

		bool file_exists_before = file_does_exist(sanitized_pathname);
		struct stat new_file;



		freopen_result= original_freopen(pathname, mode, stream);

		if(freopen_result == NULL){
			zlogf_time(ZLOG_INFO_LOG_MSG, "[!] FREOPEN ERROR: %s\n", strerror(errno));
		} else {
			if(!file_exists_before && !fstat(fileno(freopen_result), &new_file)){
			// A new file has been created.
				upsert_inode_in_array(&g_array, sanitized_pathname, get_inode(sanitized_pathname), g_temp_dir);
			}
		}
	} else {
		freopen_result= original_freopen(pathname, mode, stream);
	}

	return freopen_result;
}

int mkfifo(const char *pathname, mode_t mode){

	if(original_mkfifo == NULL){
		original_mkfifo = dlsym_wrapper(__func__);
	}

	int mkfifo_result;

	if(LIBRARY_ON){

		const char *sanitized_pathname = sanitize_and_get_absolute_path(pathname);

		print_function_and_path(__func__, pathname, sanitized_pathname);

		check_parameters_properties(pathname, __func__);



		mkfifo_result = original_mkfifo(pathname, mode);

    /*
		On success mkfifo() and mkfifoat() return 0.  In the case of an
    	error, -1 is returned (in which case, errno is set appropriately).
    */
		if(mkfifo_result == -1){
			zlogf_time(ZLOG_INFO_LOG_MSG, "[!] MKFIFO ERROR: %s\n", strerror(errno));
		} else {

			upsert_inode_in_array(&g_array, pathname, get_inode(pathname), g_temp_dir);

		}
	} else {
		mkfifo_result = original_mkfifo(pathname, mode);
	}

	return mkfifo_result;
}

int mkfifoat(int dirfd, const char *pathname, mode_t mode){

	if(original_mkfifoat == NULL){
		original_mkfifoat = dlsym_wrapper(__func__);
	}

	int mkfifoat_result;

	if(LIBRARY_ON){
		const char *sanitized_path;
		if(path_is_absolute(pathname)){
			sanitized_path = sanitize_and_get_absolute_path(pathname);
		} else {
			sanitized_path = sanitize_and_get_absolute_path_from_dir_file_descriptor(pathname, dirfd);
		}

		print_function_and_path(__func__, pathname, sanitized_path);

		check_parameters_properties(sanitized_path, __func__);

		mkfifoat_result = original_mkfifoat(dirfd, pathname, mode);

    /*
		On success, these calls return the number of bytes placed in buf.
       	(If the returned value equals bufsiz, then truncation may have
       	occurred.)  On error, -1 is returned and errno is set to indicate the
       	error.
    */
		if(mkfifoat_result == -1){
			zlogf_time(ZLOG_INFO_LOG_MSG, "[!] READLINKAT ERROR: %s\n", strerror(errno));
		} else {

			upsert_inode_in_array(&g_array, sanitized_path, get_inode(sanitized_path), g_temp_dir);

		}
	} else {
		mkfifoat_result = original_mkfifoat(dirfd, pathname, mode);
	}

	return mkfifoat_result;
}

int chmod(const char *pathname, mode_t mode){


	if(original_chmod == NULL){
		original_chmod = dlsym_wrapper(__func__);
	}

	int chmod_result;

	if(LIBRARY_ON){

		const char *sanitized_pathname = sanitize_and_get_absolute_path(pathname);

		print_function_and_path(__func__, pathname, sanitized_pathname);

		check_parameters_properties(sanitized_pathname, __func__);


		chmod_result =  original_chmod(pathname, mode);

		if(chmod_result == -1){
			zlogf_time(ZLOG_INFO_LOG_MSG, "[!] CHMOD ERROR: %s\n", strerror(errno));
		}
	} else {
		chmod_result =  original_chmod(pathname, mode);
	}

	return chmod_result;

}

int chown(const char *pathname, uid_t owner, gid_t group){

	if(original_chown == NULL){
		original_chown = dlsym_wrapper(__func__);
	}

	int chown_result;

	if(LIBRARY_ON){

		const char *sanitized_pathname = sanitize_and_get_absolute_path(pathname);

		print_function_and_path(__func__, pathname, sanitized_pathname);

		check_parameters_properties(sanitized_pathname, __func__);



		chown_result =  original_chown(pathname, owner, group);

		if(chown_result == -1){
			zlogf_time(ZLOG_INFO_LOG_MSG, "[!] CHOWN ERROR: %s\n", strerror(errno));
		}
	} else {
		chown_result =  original_chown(pathname, owner, group);
	}

	return chown_result;

}

int truncate(const char *path, off_t length){

	if(original_truncate == NULL){
		original_truncate = dlsym_wrapper(__func__);
	}

	int truncate_result;

	if(LIBRARY_ON){

		const char *sanitized_pathname = sanitize_and_get_absolute_path(path);

		print_function_and_path(__func__, path, sanitized_pathname);

		check_parameters_properties(sanitized_pathname, __func__);



		truncate_result = original_truncate(path, length);

		if(truncate_result == -1){
			zlogf_time(ZLOG_INFO_LOG_MSG, "[!] TRUNCATE ERROR: %s\n", strerror(errno));
		}
	} else {
		truncate_result = original_truncate(path, length);
	}

	return truncate_result;
}

int truncate64(const char *path, off_t length){

	if(original_truncate64 == NULL){
		original_truncate64 = dlsym_wrapper(__func__);
	}

	int truncate64_result;

	if(LIBRARY_ON){

		const char *sanitized_pathname = sanitize_and_get_absolute_path(path);

		print_function_and_path(__func__, path, sanitized_pathname);

		check_parameters_properties(sanitized_pathname, __func__);

		truncate64_result = original_truncate64(path, length);

		if(truncate64_result == -1){
			zlogf_time(ZLOG_INFO_LOG_MSG, "[!] TRUNCATE64 ERROR: %s\n", strerror(errno));
		}

	} else {
		truncate64_result = original_truncate64(path, length);
	}

	return truncate64_result;
}

int utime(const char *filename, const struct utimbuf *times){

	if(original_utime == NULL){
		original_utime = dlsym_wrapper(__func__);
	}

	int utime_result;

	if(LIBRARY_ON){


		const char *sanitized_filename = sanitize_and_get_absolute_path(filename);

		print_function_and_path(__func__, filename, sanitized_filename);

		check_parameters_properties(sanitized_filename, __func__);



		utime_result = original_utime(filename, times);

		if(utime_result == -1){
			zlogf_time(ZLOG_INFO_LOG_MSG, "[!] UTIME ERROR: %s\n", strerror(errno));
		}
	} else {
		utime_result = original_utime(filename, times);
	}

	return utime_result;
}

int utimes(const char *filename, const struct timeval *times){

	if(original_utimes == NULL){
		original_utimes = dlsym_wrapper(__func__);
	}

	int utimes_result;

	if(LIBRARY_ON){

		const char *sanitized_filename = sanitize_and_get_absolute_path(filename);

		print_function_and_path(__func__, filename, sanitized_filename);

		check_parameters_properties(sanitized_filename, __func__);

		utimes_result = original_utimes(filename, times);

		if(utimes_result == -1){
			zlogf_time(ZLOG_INFO_LOG_MSG, "[!] UTIMES ERROR: %s\n", strerror(errno));
		}
	} else {
		utimes_result = original_utimes(filename, times);
	}

	return utimes_result;
}

long pathconf(const char *path, int name){


	if(original_pathconf == NULL){
		original_pathconf = dlsym_wrapper(__func__);
	}

	int pathconf_result;

	if(LIBRARY_ON){

		const char *sanitized_path = sanitize_and_get_absolute_path(path);

		print_function_and_path(__func__, path, sanitized_path);

		check_parameters_properties(sanitized_path, __func__);


		pathconf_result = original_pathconf(path, name);

		if(pathconf_result == -1){
			zlogf_time(ZLOG_INFO_LOG_MSG, "[!] PATHCONF ERROR: %s\n", strerror(errno));
		}

	} else {
		pathconf_result = original_pathconf(path, name);
	}

	return pathconf_result;
}

int mkdir(const char *pathname, mode_t mode){

	int mkdir_result;

	if(LIBRARY_ON){

		const char *sanitized_pathname = sanitize_and_get_absolute_path(pathname);

		print_function_and_path(__func__, pathname, sanitized_pathname);

		check_parameters_properties(sanitized_pathname, __func__);

		mkdir_result = mkdir_wrapper(pathname, mode);

		if(mkdir_result == -1){
			zlogf_time(ZLOG_INFO_LOG_MSG, "[!] MKDIR ERROR: %s\n", strerror(errno));
		} else {
			upsert_inode_in_array(&g_array, sanitized_pathname, get_inode(sanitized_pathname), g_temp_dir);
		}

	} else {
		mkdir_result = mkdir_wrapper(pathname, mode);
	}

	return mkdir_result;
}

int mkdirat(int dirfd, const char *pathname, mode_t mode){

	if(original_mkdirat == NULL){
		original_mkdirat = dlsym_wrapper(__func__);
	}

	int mkdirat_result;

	if(LIBRARY_ON){

		const char *sanitized_path;
		if(path_is_absolute(pathname)){
			sanitized_path = sanitize_and_get_absolute_path(pathname);
		} else {
			sanitized_path = sanitize_and_get_absolute_path_from_dir_file_descriptor(pathname, dirfd);
		}

		print_function_and_path(__func__, pathname, sanitized_path);

		check_parameters_properties(sanitized_path, __func__);

		mkdirat_result = original_mkdirat(dirfd, pathname, mode);

		if(mkdirat_result == -1){
			zlogf_time(ZLOG_INFO_LOG_MSG, "[!] MKDIRAT ERROR: %s\n", strerror(errno));
		} else {
			upsert_inode_in_array(&g_array, sanitized_path, get_inode(sanitized_path), g_temp_dir);
		}
	} else {
		mkdirat_result = original_mkdirat(dirfd, pathname, mode);
	}

	return mkdirat_result;
}

int chdir(const char *path){


	int chdir_result;

	if(LIBRARY_ON){

		const char *sanitized_path = sanitize_and_get_absolute_path(path);

		print_function_and_path(__func__, path, sanitized_path);

		check_parameters_properties(sanitized_path, __func__);

		chdir_result = chdir_wrapper(path);

		if(chdir_result == -1){
			zlogf_time(ZLOG_INFO_LOG_MSG, "[!] CHDIR ERROR: %s\n", strerror(errno));
		}

	} else {
		chdir_result = chdir_wrapper(path);
	}

	return chdir_result;

}

int chroot(const char *path){

	int chroot_result;


	if(LIBRARY_ON){

		const char *sanitized_path = sanitize_and_get_absolute_path(path);

		print_function_and_path(__func__, path, sanitized_path);

		if(original_chroot == NULL){
			original_chroot = dlsym_wrapper(__func__);
		}

		check_parameters_properties(sanitized_path, __func__);

		chroot_result = original_chroot(path);

		if(chroot_result == -1){
			zlogf_time(ZLOG_INFO_LOG_MSG, "[!] CHROOT ERROR: %s\n", strerror(errno));
		}

	} else {
		chroot_result = original_chroot(path);
	}

	return chroot_result;

}

int execl(const char *pathname, const char *arg, ...){

	const char *sanitized_pathname = sanitize_and_get_absolute_path(pathname);

	print_function_and_path(__func__, pathname, sanitized_pathname);

	check_parameters_properties(sanitized_pathname, __func__);

	va_list variable_arguments;
	va_start(variable_arguments, arg);

	int execl_result = execlX_wrapper(0, pathname, arg, variable_arguments);

	// If execl succeeds this code will never be executed

	va_end(variable_arguments);

	if(execl_result == -1){
		zlogf_time(ZLOG_INFO_LOG_MSG, "[!] EXECL ERROR: %s\n", strerror(errno));
	}

	return execl_result;

}

int execlp(const char *file, const char *arg, ...){

	int execlp_result;

	if(LIBRARY_ON){

		const char *sanitized_file = sanitize_and_get_absolute_path(file);

		print_function_and_path(__func__, file, sanitized_file);

		check_parameters_properties(sanitized_file, __func__);

		va_list variable_arguments;
		va_start(variable_arguments, arg);

		execlp_result = execlX_wrapper(1, file, arg, variable_arguments);

	// If execlp succeeds this code will never be executed

		va_end(variable_arguments);

		if(execlp_result == -1){
			zlogf_time(ZLOG_INFO_LOG_MSG, "[!] EXECLP ERROR: %s\n", strerror(errno));
		}
	} else {
		va_list variable_arguments;
		va_start(variable_arguments, arg);

		execlp_result = execlX_wrapper(1, file, arg, variable_arguments);

		va_end(variable_arguments);
	}

	return execlp_result;

}

int execle(const char *pathname, const char *arg, ...){

	int execle_result;

	if(LIBRARY_ON){

		const char *sanitized_pathname = sanitize_and_get_absolute_path(pathname);

		print_function_and_path(__func__, pathname, sanitized_pathname);

		check_parameters_properties(sanitized_pathname, __func__);

		va_list variable_arguments;
		va_start(variable_arguments, arg);

		execle_result = execlX_wrapper(2, pathname, arg, variable_arguments);

	// If execle succeeds this code will never be executed

		va_end(variable_arguments);

		if(execle_result == -1){
			zlogf_time(ZLOG_INFO_LOG_MSG, "[!] EXECLE ERROR: %s\n", strerror(errno));
		}
	} else {
		va_list variable_arguments;
		va_start(variable_arguments, arg);
		execle_result = execlX_wrapper(2, pathname, arg, variable_arguments);
		va_end(variable_arguments);
	}

	return execle_result;

}

int execv(const char *pathname, char *const argv[]){

	int execv_result;

	if(LIBRARY_ON){

		const char *sanitized_pathname = sanitize_and_get_absolute_path(pathname);

		print_function_and_path(__func__, pathname, sanitized_pathname);

		check_parameters_properties(sanitized_pathname, __func__);

		execv_result = execv_wrapper(pathname, argv);

		if(execv_result == -1){
			zlogf_time(ZLOG_INFO_LOG_MSG, "[!] EXECV ERROR: %s\n", strerror(errno));
		}
	} else {
		execv_result = execv_wrapper(pathname, argv);
	}

	return execv_result;
}

int execvp(const char *file, char *const argv[]){

	int execvp_result;

	if(LIBRARY_ON){

		const char *sanitized_file = sanitize_and_get_absolute_path(file);

		print_function_and_path(__func__, file, sanitized_file);

		check_parameters_properties(sanitized_file, __func__);

		execvp_result = execvp_wrapper(file, argv);

	// If execv succeeds this code will never be executed

		if(execvp_result == -1){
			zlogf_time(ZLOG_INFO_LOG_MSG, "[!] EXECVP ERROR: %s\n", strerror(errno));
		}
	} else {
		execvp_result = execvp_wrapper(file, argv);
	}

	return execvp_result;

}

int execve(const char *pathname, char *const argv[], char *const envp[]){

	int execve_result;

	if(LIBRARY_ON){

		const char *sanitized_pathname = sanitize_and_get_absolute_path(pathname);

		print_function_and_path(__func__, pathname, sanitized_pathname);

		check_parameters_properties(sanitized_pathname, __func__);

		execve_result = execve_wrapper(pathname, argv, envp);

	// If execv succeeds this code will never be executed

		if(execve_result == -1){
			zlogf_time(ZLOG_INFO_LOG_MSG, "[!] EXECVE ERROR: %s\n", strerror(errno));
		}
	} else {
		execve_result = execve_wrapper(pathname, argv, envp);
	}

	return execve_result;

}

int execvpe(const char *file, char *const argv[], char *const envp[]){

	int execvpe_result;

	if(LIBRARY_ON){

		const char *sanitized_file = sanitize_and_get_absolute_path(file);

		print_function_and_path(__func__, file, sanitized_file);

		check_parameters_properties(sanitized_file, __func__);

		execvpe_result = execvpe_wrapper(file, argv, envp);

	// If execv succeeds this code will never be executed

		if(execvpe_result == -1){
			zlogf_time(ZLOG_INFO_LOG_MSG, "[!] EXECVPE ERROR: %s\n", strerror(errno));
		}

	} else {
		execvpe_result = execvpe_wrapper(file, argv, envp);
	}

	return execvpe_result;

}

FILE * popen(const char *command, const char *type){

	FILE *file;

	if(LIBRARY_ON){

		print_function_and_path(__func__, command, command);

		check_parameters_properties(command, __func__);

		if(original_popen == NULL){
			original_popen = dlsym_wrapper(__func__);
		}

		file = original_popen(command, type);

		if(file == NULL){
			zlogf_time(ZLOG_INFO_LOG_MSG, "[!] POPEN ERROR: %s\n", strerror(errno));
		}

	} else {
		file = original_popen(command, type);
	}

	return file;

}

int mount(const char *source, const char *target, const char *filesystemtype, unsigned long mountflags, const void *data){

	if(original_mount == NULL){
		original_mount = dlsym_wrapper(__func__);
	}

	int mount_result;

	if(LIBRARY_ON){

		const char *sanitized_source = sanitize_and_get_absolute_path(source);

		print_function_and_path(__func__, source, sanitized_source);

		check_parameters_properties(sanitized_source, __func__);



		mount_result = original_mount(source, target, filesystemtype, mountflags, data);

		if(mount_result == -1){
			zlogf_time(ZLOG_INFO_LOG_MSG, "[!] MOUNT ERROR: %s\n", strerror(errno));
		}

	} else {
		mount_result = original_mount(source, target, filesystemtype, mountflags, data);
	}

	return mount_result;

}

//#########################
