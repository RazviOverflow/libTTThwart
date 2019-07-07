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

#define NONEXISTING_FILE_INODE 0

#undef GET_PROGRAM_NAME
#ifdef __GLIBC__
#   define GET_PROGRAM_NAME() program_invocation_short_name
#elif defined(__APPLE__) || defined(__FreeBSD__)
#   define GET_PROGRAM_NAME() getprogname()
#else 
#	define GET_PROGRAM_NAME() "?";
#endif

// https://github.com/torvalds/linux/blob/master/include/uapi/asm-generic/fcntl.h
//#define O_RDONLY  00000000

/// ########## Hooked functions ##########

// Left-handed functions
static int (*original_xstat)(int ver, const char *path, struct stat *buf) = NULL;
static int (*original_xstat64)(int ver, const char *path, struct stat64 *buf) = NULL;
static int (*original_lxstat)(int ver, const char *path, struct stat *buf) = NULL;
static int (*original_lxstat64)(int ver, const char *path, struct stat64 *buf) = NULL;
static int (*original_access)(const char *path, int mode) = NULL;
static int (*original_rmdir)(const char *path) = NULL;
static int (*original_unlink)(const char *path) = NULL;
static int (*original_unlinkat)(int dirfd, const char *path, int flags) = NULL;
static int (*original_remove)(const char *path) = NULL;
static ssize_t (*original_readlink)(const char *pathname, char *buf, size_t bufsiz) = NULL; // readlink(2)
static ssize_t (*original_readlinkat)(int dirfd, const char *pathname, char *buf, size_t bufsiz);

// Both-handed functions
static int (*original_symlink)(const char *oldpath, const char *newpath) = NULL;
static int (*original_symlinkat)(const char *oldpath, int newdirfd, const char *newpath) = NULL;
static int (*original_link)(const char *oldpath, const char *newpath) = NULL;
static int (*original_linkat)(int olddirfd, const char *oldpath, int newdirfd, const char *newpath, int flags) = NULL;
static int (*original_rename)(const char *oldpath, const char *newpath) = NULL;
static int (*original_renameat)(int olddirfd, const char *oldpath, int newdirfd, const char *newpath) = NULL;
//static int (*original_renameat2)(int olddirfd, const char *oldpath, int newdirfd, const char *newpath, unsigned int flags) = NULL;
static int (*original_creat64)(const char *path, mode_t mode) = NULL;
static int (*original_creat)(const char *path, mode_t mode) = NULL;
static int (*original_open)(const char *path, int flags, ...) = NULL; 
static int (*original_open64)(const char *path, int flags, ...) = NULL; 
static int (*original_openat)(int dirfd, const char *path, int flags, ...) = NULL;
static FILE *(*original_fopen)(const char *path, const char *mode) = NULL;
static FILE *(*original_fopen64)(const char *path, const char *mode) = NULL;
static FILE *(*original_freopen)(const char *pathname, const char *mode, FILE *stream) = NULL;
static int (*original_xmknod)(int ver, const char *path, mode_t mode, dev_t *dev) = NULL;
static int (*original_xmknodat)(int ver, int dirfd, const char *path, mode_t mode, dev_t *dev) = NULL;
static int (*original_mkfifo)(const char *pathname, mode_t mode) = NULL;
static int (*original_mkfifoat)(int dirfd, const char *pathname, mode_t mode) = NULL;
static int (*original_chmod)(const char *pathname, mode_t mode) = NULL;
static int (*original_chown)(const char *pathname, uid_t owner, gid_t group) = NULL;
static int (*original_truncate)(const char *path, off_t length) = NULL;
static int (*original_truncate64)(const char *path, off64_t length) = NULL;
static int (*original_utime)(const char *filename, const struct utimbuf *times) = NULL;
static int (*original_utimes)(const char *filename, const struct timeval times[2]) = NULL;
static long(*original_pathconf)(const char *path, int name) = NULL;
static int (*original_mkdir)(const char *pathname, mode_t mode) = NULL;
static int (*original_mkdirat)(int dirfd, const char *pathname, mode_t mode) = NULL;
static int (*original_chdir)(const char *path) = NULL;
static int (*original_chroot)(const char *path) = NULL;
//static int (*original_pivot_root)(const char *new_root, const char *putold) = NULL;

/* execl* family is being hooked but there is no need for particular pointers 
	to functions since they internally call execv, execvp and execve respectively*/
//static int (*original_execl)(const char *pathname, const char *arg, ...) = NULL;
//static int (*original_execlp)(const char *file, const char *arg, ...) = NULL;
//static int (*original_execle)(const char *pathname, const char *arg, ...) = NULL;

//static int (*original_execv)(const char *pathname, char *const argv[]) = NULL;
//static int (*original_execvp)(const char *file, char *const argv[]) = NULL;
static int (*original_execve)(const char *pathname, char *const argv[], char *const envp[]) = NULL;
static int (*original_execvpe)(const char *file, char *const argv[], char *const envp[]) = NULL;

// doubts
static FILE *(*original_popen)(const char *command, const char *type) = NULL;
//static int (*original_pclose)(FILE *stream) = NULL;
static int (*original_mount)(const char *source, const char *target, const char *filesystemtype, unsigned long mountflags, const void *data) = NULL;

/// ########## Hooked functions ##########

/// <-------------------------------------------------> 

/// ########## Coconstructor and Destructor ##########
static void before_main(void) __attribute__((constructor));
static void after_main(void) __attribute__((destructor));
/// ########## Coconstructor and Destructor ##########

/// <-------------------------------------------------> 

/// ########## Prototype declaration ##########

void check_parameters_properties(const char *, const char *);
void * dlsym_wrapper(const char *);
int open_wrapper(const char *, int, va_list);
int openat_wrapper(int, const char *, int, va_list);
int execlX_wrapper(int, const char *, const char *, va_list);
int execv_wrapper(const char *, char * const *);
int execvp_wrapper(const char *, char * const *);
int execve_wrapper(const char *, char * const *, char * const *);
int execvpe_wrapper(const char *, char * const *, char * const *);
int chdir_wrapper(const char *);
ino_t get_inode(const char *);
void print_function_and_path(const char *, const char *, const char *);
const char * sanitize_and_get_absolute_path(const char *);
const char * sanitize_and_get_absolute_path_from_dir_file_descriptor(const char *, int);
//void timestamp();
int file_does_exist(const char *);
char * get_directory_from_fd(int);
bool path_is_absolute(const char *);
void create_log_dir_and_start_logger();
void create_log_file_and_start_logger(const char *);

/// ########## Prototype declaration ##########

/// <-------------------------------------------------> 

/// ########## file_objects_info.c ##########
typedef struct{
	char *path;
	ino_t inode;
	int fd_number;
} file_object_info;

typedef struct{
	file_object_info *list;
	size_t used;
	size_t size;
} file_objects_info;

// -- Array operations -- //
void initialize_array(file_objects_info *, size_t);
void upsert_inode_in_array(file_objects_info *, const char *, ino_t);
void free_array(file_objects_info *);
int find_index_in_array(file_objects_info *, const char *);
file_object_info get_from_array_at_index(file_objects_info *, int);
void remove_from_array_at_index(file_objects_info *, int);

/// ########## file_objects_info.c ##########

/// <------------------------------------------------->

/// ########## GLOBAL VARIABLES ##########
file_objects_info g_array;
bool LIBRARY_ON; //defaults to false
/// ########## GLOBAL VARIABLES ##########

/// <-------------------------------------------------> 

/// ########## Coconstructor and Destructor ##########

static void before_main(void){

	//printf("PATH Variable environment: %s\n", getenv("PATH"));
	//printf("ENVIRON: %s\n", *__environ);

	if(getuid() != geteuid()){
		LIBRARY_ON = true;
	}

	if(LIBRARY_ON){
		create_log_dir_and_start_logger();

		//zlog_init_stdout();

		zlogf_time(ZLOG_DEBUG_LOG_MSG, "[+] I AM %s w/ PID: %d and PPID: %d [+]\n", GET_PROGRAM_NAME(), getpid(), getppid());
		zlog_flush_buffer();
	}
}

static void after_main(void){

	if(LIBRARY_ON){
		zlogf_time(ZLOG_DEBUG_LOG_MSG,"[+] I WAS  %s w/ PID: %d and PPID: %d [+]\n", GET_PROGRAM_NAME(), getpid(), getppid());
		zlog_flush_buffer();
	}
	free_array(&g_array);

}

/// ########## Coconstructor and Destructor ##########

/// <-------------------------------------------------> 

/// ########## Logger ##########

void create_log_dir_and_start_logger(){

	char *home = getenv("HOME");
	if(!home){
		fprintf(stderr, "[!] ERROR RETRIEVING HOME ENV VARIABLE.\n[!] ERROR: %s\n[!] ALL LOGS WILL BE REDIRECTED TO STDERR (2).\n", strerror(errno));
		zlog_init_stderr();
		return;
	} else {

		char *folder_name = "libTOCTTOU";

		int log_folder_dir_length = strlen(home) + strlen (folder_name) + 2;
		char log_folder_dir[log_folder_dir_length];
		snprintf(log_folder_dir, log_folder_dir_length, "%s/%s", home, folder_name);

		DIR *folder_dir = opendir(log_folder_dir);
		if(folder_dir){
			closedir(folder_dir);
		} else {
			if(mkdir(log_folder_dir, 0755) == -1){ // (Bear in mind umask)
				fprintf(stderr, "[!] ERROR CREATING LOG DIRECTORY.\n[!] ERROR: %s\n[!] ALL LOGS WILL BE REDIRECTED TO STDERR (2).\n", strerror(errno));
				zlog_init_stderr();
				return;
			}
			closedir(folder_dir);
		}



		char *program_name = GET_PROGRAM_NAME();
		// +3 because of "/" and null trailling byte
		int log_dir_length = strlen(home) + strlen (folder_name) + strlen(program_name) + 3;
		char log_dir[log_dir_length];

		snprintf(log_dir, log_dir_length, "%s/%s/%s", home, folder_name, program_name);
		
		DIR *dir = opendir(log_dir);
		if(dir){ //Directory exists
			closedir(dir);
			create_log_file_and_start_logger(log_dir);
		} else if(errno == ENOENT){ //Directory does not exist; create
			if(mkdir(log_dir, 0755) == -1){ // (Bear in mind umask)
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
	
}


void create_log_file_and_start_logger(const char *log_dir){
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

}


/// ########## Logger ##########

/// <-------------------------------------------------> 

/// ########## Array management ##########
void check_dlsym_error(){
	char * error = dlerror();
	if(error != NULL){
		exit(EXIT_FAILURE);
	}
}


/*
    Initializes the given array with the given size, allocating
    the corresponding memory.
*/
void initialize_array(file_objects_info *array, size_t size){

	array->list = (file_object_info *) calloc(size, sizeof(file_object_info)); 
	if(!array->list){
		exit(EXIT_FAILURE);
	}
	array->used = 0;
	array->size = size;

}

/*
    Inserts into the given array the given path and inode.  
    Before inserting elements into the given array, the array 
    must be initialized.
    If there is not enough room in the array to insert a new
    file_object_info element, the size of the array gets doubled.
    After the element is inserted, "used" member of the given
    array is postincremented.
    If the path already exists in the array, the function 
    updates the corresponding inode instead of inserting new
    element.
*/
void upsert_inode_in_array(file_objects_info *array, const char *path, ino_t inode){
    
    // If array has not been yet initialized, initialize it. 
	if(array->size == 0){
		initialize_array(&g_array, 2);
	} 
	// If element is already in array simply update its inode in case the
	// the new inode is different from the one already existing
	int index = find_index_in_array(array, path);

	if(index >= 0){
		if(inode != array->list[index].inode){
			array->list[index].inode = inode;
			zlogf_time(ZLOG_DEBUG_LOG_MSG, "Updated inode (now %lu) of path %s\n", inode, path);
		}
	} else  {
    // If number of elements (used) in the array equals its size, it means
    // the array requires more room. It's size gets doubled
		if(array->used == array->size){
		////printf("Size of array %X is about to get doubled.\n", &(*array));
		//printf("Size of array is about to get doubled\n");
			array->size *= 2;
			file_object_info *aux = (file_object_info *)realloc(array->list,
				array->size * sizeof(file_object_info));

        // It is never a good idea to do something like:
        // array->list = realloc... because if realloc fails you lose the
        // reference to your original data and realloc does not free() so
        // there'll be an implicit memory leak.
			if(!aux){
				fprintf(stderr, "Error trying to realloc size for array in upsert inode process.\n");
				exit(EXIT_FAILURE);
			} else {
				array->list = aux;
			}

        //Initializing new elements of realocated array
			memset(&array->list[array->used], 0, sizeof(file_object_info) * (array->size - array->used));

		}
		array->list[array->used].path = strdup(path);
		array->list[array->used].inode = inode;
		array->list[array->used].fd_number = 0;
		array->used++;
	}

}

/*
    Frees the memory used by the given array. This function
    is ment to be called at the end of the program.
*/
void free_array(file_objects_info *array){

	for(uint i = 0; i < array->used; i++){
		free(array->list[i].path);
		array->list[i].path = NULL;
	}

	free(array->list);
	array->list = NULL;

	array->used = 0;
	array->size = 0;

}

/*
    Find the index of the given path in the given array. If array's
    size is not bigger than 0, it means the array has not yet been 
    initialized, so there is no way the element could be found. 
*/
int find_index_in_array(file_objects_info *array, const char *path){

	int returnValue = -1;

	if(array->size > 0){
		for(uint i = 0; i < array->used; i++){
			if(!strcmp(array->list[i].path, path)){
				returnValue = i;
			break;
			}
		}
		return returnValue;
	} else {
		return returnValue;
	}
}

/*
   Retrieve the file_object_info element at the given index in the
   given array. 
*/
file_object_info get_from_array_at_index(file_objects_info *array, int index){
	return array->list[index];
}

/*
	Removes the element at index index from the array. Please note it's index,
	not position. Index starts at 0.
*/
void remove_from_array_at_index(file_objects_info *array, int index){

	int number_elements = array->used;
	if(index < number_elements){

		for(int i = index; i < number_elements; i++){
			array->list[i] = array->list[i+1];
		}

	} 

	array->used--;
}

/// ########## Array management ##########

/// <-------------------------------------------------> 

/// ########## Core and useful functions ##########
void print_function_and_path(const char* func, const char* path, const char* sanitized_path){
	zlogf_time(ZLOG_DEBUG_LOG_MSG, "[+] User invoked %s via process %s on: %s\n", func, GET_PROGRAM_NAME(), path);
	zlogf_time(ZLOG_DEBUG_LOG_MSG, "[+] Sanitized: %s\n", sanitized_path);
}

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
			upsert_inode_in_array(&g_array, path, inode);
		} else {
			file_object_info aux = get_from_array_at_index(&g_array,index);
			if(aux.inode != inode){
			//printf("FILE %s EXISTS: %d\n", path, exists);
				zlogf_time(ZLOG_INFO_LOG_MSG, "[+][!] WARNING! TOCTTOU DETECTED! [+][!]\n Inode of <%s> has changed since it was previously invoked. Threat detected when invoking <%s> function. Inode was <%lu> and now it is <%lu>. \n [#] PROGRAM %s ABORTED [#]\n\n", path, caller_function_name, aux.inode, inode, GET_PROGRAM_NAME());
				fprintf(stderr,"[+][!] WARNING! TOCTTOU DETECTED!. [!][+]\n[#] PROGRAM %s ABORTED [#]\n[#] Check logs for more info [#]\n", GET_PROGRAM_NAME());
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
		upsert_inode_in_array(&g_array, path, NONEXISTING_FILE_INODE);
	}
}

/*
    The correct way to test for an error is to call dlerror() 
    to clear any old error conditions, then call dlsym(), and 
    then call dlerror() again, saving its return value into a
    variable, and check whether this saved value is not NULL.
    https://linux.die.net/man/3/dlsym
*/
void* dlsym_wrapper(const char *original_function){

	dlerror();

	void *function_handler;

	function_handler = dlsym(RTLD_NEXT, original_function);

	check_dlsym_error();

	return function_handler;
}

/*
    The open wrapper guarantees, insures original_open is initialized.
    It's used by other inner functions in order to avoid open() recursivity
    and overhead. In adittion, it deals with ellipsis (variable 
    arguments) since open is a variadic function.
*/
int open_wrapper(const char *path, int flags, va_list variable_arguments){

	if ( original_open == NULL ) {
		original_open = dlsym_wrapper("open");
	}

	if(variable_arguments){
		va_list aux_list;
		va_copy(aux_list, variable_arguments);

		mode_t mode = va_arg(aux_list, mode_t);

		va_end(aux_list);

		return original_open(path, flags, mode);

	} else {
		return original_open(path, flags);
	}
	
}

int open64_wrapper(const char *path, int flags, va_list variable_arguments){

	if ( original_open64 == NULL ) {
		original_open64 = dlsym_wrapper("open64");
	}

	if(variable_arguments){
		va_list aux_list;
		va_copy(aux_list, variable_arguments);

		mode_t mode = va_arg(aux_list, mode_t);

		va_end(aux_list);

		return original_open64(path, flags, mode);

	} else {
		return original_open64(path, flags);
	}
	
}

/*
	Openat wrapper is exactly the same as open wrapper but for
	 openat.
*/
int openat_wrapper(int dirfd, const char *path, int flags, va_list variable_arguments){

	if(original_openat == NULL){
		original_openat = dlsym_wrapper("openat");
	}

	if(variable_arguments){
		va_list aux_list;
		va_copy(aux_list, variable_arguments);

		mode_t mode = va_arg(variable_arguments, mode_t);

		va_end(aux_list);

		return original_openat(dirfd, path, flags, mode);

	} else {
		return original_openat(dirfd, path, flags);
	}

}

/*
	Same as open_wrapper.
*/
int chdir_wrapper(const char *path){
	
	if(original_chdir == NULL){
		original_chdir = dlsym_wrapper("chdir");
	}

	return original_chdir(path);
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
	Wrapper for all execlX functions family. This wrapper treats the variable
	arguments and calls the corresponding execlX function according to:
	[function argument value] : [execlX function]
	0 : execl https://code.woboq.org/userspace/glibc/posix/execl.c.html
	1 : execlp https://code.woboq.org/userspace/glibc/posix/execlp.c.html
	2 : execle https://code.woboq.org/userspace/glibc/posix/execle.c.html

	Additional info: https://code.woboq.org/userspace/glibc/posix/execl.c.html
*/
int execlX_wrapper(int function, const char *pathname, const char *arg, va_list variable_arguments){
	int execlX_result = -1;

	if(function == 0 || function == 1 ){
		va_list aux_list;
		va_copy(aux_list, variable_arguments);
		int number_of_arguments = get_number_of_variable_arguments_char_pointer_type(aux_list);
		if(number_of_arguments == -1){
			fprintf(stderr,"Error when retrieveng variable arguments. Aborting\n. Error: %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}

		// Reset aux_list and start from the very beginning when using va_arg
		va_end(aux_list);
		va_copy(aux_list, variable_arguments);

		char *argv[number_of_arguments + 1];
		argv[0] = (char *) arg;
		ptrdiff_t i;
		for(i = 1; i<= number_of_arguments; i++){
			argv[i] = va_arg(aux_list, char *);
		}

		va_end(aux_list);
		switch(function){
			case 0:
				execlX_result = execve_wrapper(pathname, argv, __environ);
				break;
			case 1:
				execlX_result = execvpe_wrapper(pathname, argv, __environ);
				break;
		}

	} else if(function == 2){

		va_list aux_list;
		va_copy(aux_list, variable_arguments);
		int number_of_arguments = get_number_of_variable_arguments_char_pointer_type(aux_list);
		if(number_of_arguments == -1){
			fprintf(stderr,"Error when retrieveng variable arguments. Aborting\n. Error: %s\n", strerror(errno));
			exit(EXIT_FAILURE);		
		}

		// Reset aux_list and start from the very beginning when using va_arg
		va_end(aux_list);
		va_copy(aux_list, variable_arguments);

		char *argv[number_of_arguments + 1];
		char **envp;
		argv[0] = (char *) arg;
		ptrdiff_t i;
		for(i = 1; i<= number_of_arguments; i++){
			argv[i] = va_arg(aux_list, char *);
		}
		envp = va_arg(variable_arguments, char **);
		va_end(aux_list);

		execlX_result = execve_wrapper(pathname, argv, envp);
	} 

	return execlX_result;
}

int execv_wrapper(const char *pathname, char *const argv[]){

	return execve_wrapper(pathname, argv, __environ);
}

int execvp_wrapper(const char *file, char *const argv[]){

	return execvpe_wrapper(file, argv, __environ);
}

int execve_wrapper(const char *pathname, char *const argv[], char *const envp[]){
	if ( original_execve == NULL ) {
		original_execve = dlsym_wrapper("execve");
	}

	return original_execve(pathname, argv, envp);
}

int execvpe_wrapper(const char *file, char *const argv[], char *const envp[]){
	if ( original_execvpe == NULL ) {
		original_execvpe = dlsym_wrapper("execvpe");
	}

	return original_execvpe(file, argv, envp);
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


/// ########## Core and useful functions ##########

/// <-------------------------------------------------> 

/// ########## Hooked functions replacement code ##########

int __xstat(int ver, const char *path, struct stat *buf)
{

	if(LIBRARY_ON){
		const char *sanitized_path = sanitize_and_get_absolute_path(path);

		print_function_and_path(__func__, path, sanitized_path);

		upsert_inode_in_array(&g_array, sanitized_path, get_inode(sanitized_path));

	}
	if ( original_xstat == NULL ) {
		original_xstat = dlsym_wrapper(__func__);
	}
	return original_xstat(ver, path, buf);
} 

int __xstat64(int ver, const char *path, struct stat64 *buf)
{

	if(LIBRARY_ON){
		const char *sanitized_path = sanitize_and_get_absolute_path(path);

		print_function_and_path(__func__, path,sanitized_path);

		upsert_inode_in_array(&g_array, sanitized_path, get_inode(sanitized_path));
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

		upsert_inode_in_array(&g_array, sanitized_path, get_inode(sanitized_path));
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

		upsert_inode_in_array(&g_array, sanitized_path, get_inode(sanitized_path));
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

				upsert_inode_in_array(&g_array, sanitized_path, inode);

			}

		}
	} else {

		va_list variable_arguments;
		va_start(variable_arguments, flags);

		open_result = open_wrapper(path, flags, variable_arguments);

		va_end(variable_arguments);
	}

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

				upsert_inode_in_array(&g_array, sanitized_path, inode);

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

		upsert_inode_in_array(&g_array, sanitized_path, get_inode(sanitized_path));
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



		fopen_result = original_fopen(path, mode);

		if(fopen_result == NULL){
			zlogf_time(ZLOG_INFO_LOG_MSG, "[!] FOPEN ERROR: %s\n", strerror(errno));
		} else {
			if(!path_exists_before && !fstat(fileno(fopen_result), &new_file)){
			// A new file has been created.
				upsert_inode_in_array(&g_array, sanitized_path, get_inode(sanitized_path));
			}
		}
	} else {
		fopen_result = original_fopen(path, mode);
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
			
			upsert_inode_in_array(&g_array, sanitized_path, inode);
			

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


	if(original_symlink == NULL){
    	original_symlink = dlsym_wrapper(__func__);
    }

    int symlink_result;

	if(LIBRARY_ON){
	
    const char *sanitized_newpath = sanitize_and_get_absolute_path(newpath);

   	print_function_and_path(__func__, newpath, sanitized_newpath);

   	check_parameters_properties(sanitized_newpath, __func__);
   	
   

    symlink_result = original_symlink(oldpath, newpath);

    /*
		Upon successful completion, symlink() shall return 0; otherwise, it 
		shall return -1 and set errno to indicate the error.
    */
    if(symlink_result == -1){
		zlogf_time(ZLOG_INFO_LOG_MSG, "[!] SYMLINK ERROR: %s\n", strerror(errno));
	} else {

	    ino_t inode = get_inode(sanitized_newpath);

		upsert_inode_in_array(&g_array, sanitized_newpath, inode);
		
	}
}
else{
	symlink_result = original_symlink(oldpath, newpath);
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
		upsert_inode_in_array(&g_array, sanitized_new_path, inode);
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

			upsert_inode_in_array(&g_array, sanitized_path, inode);

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
			upsert_inode_in_array(&g_array, sanitized_path, inode);
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

	if(original_link == NULL){
		original_link = dlsym_wrapper(__func__);
	}

	int link_result;

	if(LIBRARY_ON){

		const char *sanitized_new_path = sanitize_and_get_absolute_path(newpath);

		print_function_and_path(__func__, newpath, sanitized_new_path);

		check_parameters_properties(sanitized_new_path, __func__);



		link_result = original_link(oldpath, newpath);

   	/*
		Upon successful completion, 0 shall be returned. Otherwise, -1 shall
		be returned and errno set to indicate the error.
   	*/
		if(link_result == -1){
			zlogf_time(ZLOG_INFO_LOG_MSG, "[!] LINK ERROR: %s\n", strerror(errno));
		} else {

			ino_t inode = get_inode(sanitized_new_path);

			upsert_inode_in_array(&g_array, sanitized_new_path, inode);

		}

	} else {
		link_result = original_link(oldpath, newpath);
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
			upsert_inode_in_array(&g_array, full_new_path, inode);

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
			upsert_inode_in_array(&g_array, sanitized_path, get_inode(sanitized_path));
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
			upsert_inode_in_array(&g_array, sanitized_path, get_inode(sanitized_path));
		}
	} else {
		creat_result = original_creat(path, mode);
	}

	return creat_result;


}

int rmdir(const char *path){
    
    const char *sanitized_path = sanitize_and_get_absolute_path(path);

	print_function_and_path(__func__, path, sanitized_path);

	check_parameters_properties(sanitized_path, __func__);

	if(original_rmdir == NULL){
    	original_rmdir = dlsym_wrapper(__func__);
    }

	int rmdir_result = original_rmdir(path);

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

    return rmdir_result;

}

ssize_t readlink(const char *pathname, char *buf, size_t bufsiz){

	const char* sanitized_pathname = sanitize_and_get_absolute_path(pathname);

	print_function_and_path(__func__, pathname, sanitized_pathname);

	if ( original_readlink == NULL ) {
		original_readlink = dlsym_wrapper(__func__);
	}

	int readlink_result = original_readlink(pathname, buf, bufsiz);

	upsert_inode_in_array(&g_array, sanitized_pathname, get_inode(sanitized_pathname));

	if(readlink_result == -1){
    	zlogf_time(ZLOG_INFO_LOG_MSG, "[!] READLINK ERROR: %s\n", strerror(errno));
    } else {

    	ino_t inode = get_inode(sanitized_pathname);
		upsert_inode_in_array(&g_array, sanitized_pathname, inode);

    }

	return readlink_result;
}

ssize_t readlinkat(int dirfd, const char *pathname, char *buf, size_t bufsiz){


	if(original_readlinkat == NULL){
		original_readlinkat = dlsym_wrapper(__func__);
	}

	const char *sanitized_path;
	if(path_is_absolute(pathname)){
		sanitized_path = sanitize_and_get_absolute_path(pathname);
	} else {
		sanitized_path = sanitize_and_get_absolute_path_from_dir_file_descriptor(pathname, dirfd);
	}

    print_function_and_path(__func__, pathname, sanitized_path);

    int readlinkat_result = original_readlinkat(dirfd, pathname, buf, bufsiz);

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
		upsert_inode_in_array(&g_array, sanitized_path, inode);

    }

    return readlinkat_result;
}

int rename(const char *oldpath, const char *newpath){

	const char *sanitized_old_path = sanitize_and_get_absolute_path(oldpath);
	const char *sanitized_new_path = sanitize_and_get_absolute_path(newpath);

	print_function_and_path(__func__, newpath, sanitized_new_path);

	check_parameters_properties(sanitized_new_path, __func__);

	if(original_rename == NULL){
		original_rename = dlsym_wrapper(__func__);
	}

	int rename_result = original_rename(oldpath, newpath);

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
		upsert_inode_in_array(&g_array, newpath, inode);
		
	}

	return rename_result;
}

int renameat(int olddirfd, const char *oldpath, int newdirfd, const char *newpath){

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

    	if(original_renameat == NULL){
		original_renameat = dlsym_wrapper(__func__);
	}

	int renameat_result = original_renameat(olddirfd, oldpath, newdirfd, newpath);

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
		upsert_inode_in_array(&g_array, newpath, inode);

	}

	return renameat_result;
}

FILE *fopen64(const char *path, const char *mode){

	const char *sanitized_path = sanitize_and_get_absolute_path(path);

	print_function_and_path(__func__, path, sanitized_path);

	check_parameters_properties(sanitized_path, __func__);

	bool path_exists_before = file_does_exist(sanitized_path);
	struct stat new_file;

	if(original_fopen64 == NULL){
		original_fopen64 = dlsym_wrapper(__func__);
	} 

	FILE *fopen64_result = original_fopen64(path, mode);

	if(fopen64_result == NULL){
		zlogf_time(ZLOG_INFO_LOG_MSG, "[!] FOPEN64 ERROR: %s\n", strerror(errno));
	} else {
		if(!path_exists_before && !fstat(fileno(fopen64_result), &new_file)){
			upsert_inode_in_array(&g_array, sanitized_path, get_inode(sanitized_path));
		}
	}

	return fopen64_result;

}

FILE *freopen(const char *pathname, const char *mode, FILE *stream){

	const char *sanitized_pathname = sanitize_and_get_absolute_path(pathname);

	print_function_and_path(__func__, pathname, sanitized_pathname);

	check_parameters_properties(sanitized_pathname, __func__);

	bool file_exists_before = file_does_exist(sanitized_pathname);
	struct stat new_file;

	if(original_freopen == NULL){
		original_freopen = dlsym_wrapper(__func__);
	}

	FILE *freopen_result= original_freopen(pathname, mode, stream);

	if(freopen_result == NULL){
		zlogf_time(ZLOG_INFO_LOG_MSG, "[!] FREOPEN ERROR: %s\n", strerror(errno));
	} else {
		if(!file_exists_before && !fstat(fileno(freopen_result), &new_file)){
			// A new file has been created.
			upsert_inode_in_array(&g_array, sanitized_pathname, get_inode(sanitized_pathname));
		}
	}

	return freopen_result;
}

int mkfifo(const char *pathname, mode_t mode){

    const char *sanitized_pathname = sanitize_and_get_absolute_path(pathname);

   	print_function_and_path(__func__, pathname, sanitized_pathname);

	check_parameters_properties(pathname, __func__);

	if(original_mkfifo == NULL){
    	original_mkfifo = dlsym_wrapper(__func__);
    }
	
    int mkfifo_result = original_mkfifo(pathname, mode);

    /*
		On success mkfifo() and mkfifoat() return 0.  In the case of an
    	error, -1 is returned (in which case, errno is set appropriately).
    */
    if(mkfifo_result == -1){
		zlogf_time(ZLOG_INFO_LOG_MSG, "[!] MKFIFO ERROR: %s\n", strerror(errno));
	} else {
		
		upsert_inode_in_array(&g_array, pathname, get_inode(pathname));
		
	}

    return mkfifo_result;
}

int mkfifoat(int dirfd, const char *pathname, mode_t mode){


	const char *sanitized_path;
	if(path_is_absolute(pathname)){
		sanitized_path = sanitize_and_get_absolute_path(pathname);
	} else {
		sanitized_path = sanitize_and_get_absolute_path_from_dir_file_descriptor(pathname, dirfd);
	}

    print_function_and_path(__func__, pathname, sanitized_path);

	check_parameters_properties(sanitized_path, __func__);

	if(original_mkfifoat == NULL){
		original_mkfifoat = dlsym_wrapper(__func__);
	}

    int mkfifoat_result = original_mkfifoat(dirfd, pathname, mode);

    /*
		On success, these calls return the number of bytes placed in buf.
       	(If the returned value equals bufsiz, then truncation may have
       	occurred.)  On error, -1 is returned and errno is set to indicate the
       	error.
    */
    if(mkfifoat_result == -1){
    	zlogf_time(ZLOG_INFO_LOG_MSG, "[!] READLINKAT ERROR: %s\n", strerror(errno));
    } else {

		upsert_inode_in_array(&g_array, sanitized_path, get_inode(sanitized_path));

	}

    return mkfifoat_result;
}

int chmod(const char *pathname, mode_t mode){

	const char *sanitized_pathname = sanitize_and_get_absolute_path(pathname);

	print_function_and_path(__func__, pathname, sanitized_pathname);

	check_parameters_properties(sanitized_pathname, __func__);

	if(original_chmod == NULL){
		original_chmod = dlsym_wrapper(__func__);
	}

	int chmod_result =  original_chmod(pathname, mode);

	if(chmod_result == -1){
		zlogf_time(ZLOG_INFO_LOG_MSG, "[!] CHMOD ERROR: %s\n", strerror(errno));
	}

	return chmod_result;

}

int chown(const char *pathname, uid_t owner, gid_t group){

	const char *sanitized_pathname = sanitize_and_get_absolute_path(pathname);

	print_function_and_path(__func__, pathname, sanitized_pathname);

	check_parameters_properties(sanitized_pathname, __func__);

	if(original_chown == NULL){
		original_chown = dlsym_wrapper(__func__);
	}

	int chown_result =  original_chown(pathname, owner, group);

	if(chown_result == -1){
		zlogf_time(ZLOG_INFO_LOG_MSG, "[!] CHOWN ERROR: %s\n", strerror(errno));
	}

	return chown_result;

}

int truncate(const char *path, off_t length){

	const char *sanitized_pathname = sanitize_and_get_absolute_path(path);

	print_function_and_path(__func__, path, sanitized_pathname);

	check_parameters_properties(sanitized_pathname, __func__);

	if(original_truncate == NULL){
		original_truncate = dlsym_wrapper(__func__);
	}

	int truncate_result = original_truncate(path, length);

	if(truncate_result == -1){
		zlogf_time(ZLOG_INFO_LOG_MSG, "[!] TRUNCATE ERROR: %s\n", strerror(errno));
	}

	return truncate_result;
}

int truncate64(const char *path, off_t length){

	const char *sanitized_pathname = sanitize_and_get_absolute_path(path);

	print_function_and_path(__func__, path, sanitized_pathname);

	check_parameters_properties(sanitized_pathname, __func__);

	if(original_truncate64 == NULL){
		original_truncate64 = dlsym_wrapper(__func__);
	}

	int truncate64_result = original_truncate64(path, length);

	if(truncate64_result == -1){
		zlogf_time(ZLOG_INFO_LOG_MSG, "[!] TRUNCATE64 ERROR: %s\n", strerror(errno));
	}

	return truncate64_result;
}

int utime(const char *filename, const struct utimbuf *times){

	const char *sanitized_filename = sanitize_and_get_absolute_path(filename);

	print_function_and_path(__func__, filename, sanitized_filename);

	check_parameters_properties(sanitized_filename, __func__);

	if(original_utime == NULL){
		original_utime = dlsym_wrapper(__func__);
	}

	int utime_result = original_utime(filename, times);

	if(utime_result == -1){
		zlogf_time(ZLOG_INFO_LOG_MSG, "[!] UTIME ERROR: %s\n", strerror(errno));
	}

	return utime_result;
}

int utimes(const char *filename, const struct timeval *times){

	const char *sanitized_filename = sanitize_and_get_absolute_path(filename);

	print_function_and_path(__func__, filename, sanitized_filename);

	check_parameters_properties(sanitized_filename, __func__);

	if(original_utimes == NULL){
		original_utimes = dlsym_wrapper(__func__);
	}

	int utimes_result = original_utimes(filename, times);

	if(utimes_result == -1){
		zlogf_time(ZLOG_INFO_LOG_MSG, "[!] UTIMES ERROR: %s\n", strerror(errno));
	}

	return utimes_result;
}

long pathconf(const char *path, int name){

	const char *sanitized_path = sanitize_and_get_absolute_path(path);

	print_function_and_path(__func__, path, sanitized_path);

	check_parameters_properties(sanitized_path, __func__);

	if(original_pathconf == NULL){
		original_pathconf = dlsym_wrapper(__func__);
	}

	int pathconf_result = original_pathconf(path, name);

	if(pathconf_result == -1){
		zlogf_time(ZLOG_INFO_LOG_MSG, "[!] PATHCONF ERROR: %s\n", strerror(errno));
	}

	return pathconf_result;
}

int mkdir(const char *pathname, mode_t mode){

	const char *sanitized_pathname = sanitize_and_get_absolute_path(pathname);

	print_function_and_path(__func__, pathname, sanitized_pathname);

	check_parameters_properties(sanitized_pathname, __func__);

	if(original_mkdir == NULL){
		original_mkdir = dlsym_wrapper(__func__);
	}

	int mkdir_result = original_mkdir(pathname, mode);

	if(mkdir_result == -1){
		zlogf_time(ZLOG_INFO_LOG_MSG, "[!] MKDIR ERROR: %s\n", strerror(errno));
	} else {
		upsert_inode_in_array(&g_array, sanitized_pathname, get_inode(sanitized_pathname));
	}

	return mkdir_result;
}

int mkdirat(int dirfd, const char *pathname, mode_t mode){

	

	const char *sanitized_path;
	if(path_is_absolute(pathname)){
		sanitized_path = sanitize_and_get_absolute_path(pathname);
	} else {
		sanitized_path = sanitize_and_get_absolute_path_from_dir_file_descriptor(pathname, dirfd);
	}

	print_function_and_path(__func__, pathname, sanitized_path);

	check_parameters_properties(sanitized_path, __func__);

	if(original_mkdirat == NULL){
		original_mkdirat = dlsym_wrapper(__func__);
	}

	int mkdirat_result = original_mkdirat(dirfd, pathname, mode);

	if(mkdirat_result == -1){
		zlogf_time(ZLOG_INFO_LOG_MSG, "[!] MKDIRAT ERROR: %s\n", strerror(errno));
	} else {
		upsert_inode_in_array(&g_array, sanitized_path, get_inode(sanitized_path));
	}

	return mkdirat_result;
}

int chdir(const char *path){

	const char *sanitized_path = sanitize_and_get_absolute_path(path);

	print_function_and_path(__func__, path, sanitized_path);

	check_parameters_properties(sanitized_path, __func__);

	int chdir_result = chdir_wrapper(path);

	if(chdir_result == -1){
		zlogf_time(ZLOG_INFO_LOG_MSG, "[!] CHDIR ERROR: %s\n", strerror(errno));
	}

	return chdir_result;

}

int chroot(const char *path){

	const char *sanitized_path = sanitize_and_get_absolute_path(path);

	print_function_and_path(__func__, path, sanitized_path);

	if(original_chroot == NULL){
		original_chroot = dlsym_wrapper(__func__);
	}

	check_parameters_properties(sanitized_path, __func__);

	int chroot_result = original_chroot(path);

	if(chroot_result == -1){
		zlogf_time(ZLOG_INFO_LOG_MSG, "[!] CHROOT ERROR: %s\n", strerror(errno));
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

	const char *sanitized_file = sanitize_and_get_absolute_path(file);

	print_function_and_path(__func__, file, sanitized_file);

	check_parameters_properties(sanitized_file, __func__);

	va_list variable_arguments;
	va_start(variable_arguments, arg);

	int execlp_result = execlX_wrapper(1, file, arg, variable_arguments);

	// If execlp succeeds this code will never be executed

	va_end(variable_arguments);

	if(execlp_result == -1){
		zlogf_time(ZLOG_INFO_LOG_MSG, "[!] EXECLP ERROR: %s\n", strerror(errno));
	}

	return execlp_result;

}

int execle(const char *pathname, const char *arg, ...){

	const char *sanitized_pathname = sanitize_and_get_absolute_path(pathname);

	print_function_and_path(__func__, pathname, sanitized_pathname);

	check_parameters_properties(sanitized_pathname, __func__);

	va_list variable_arguments;
	va_start(variable_arguments, arg);

	int execle_result = execlX_wrapper(2, pathname, arg, variable_arguments);

	// If execle succeeds this code will never be executed

	va_end(variable_arguments);

	if(execle_result == -1){
		zlogf_time(ZLOG_INFO_LOG_MSG, "[!] EXECLE ERROR: %s\n", strerror(errno));
	}

	return execle_result;

}

int execv(const char *pathname, char *const argv[]){

	const char *sanitized_pathname = sanitize_and_get_absolute_path(pathname);

	print_function_and_path(__func__, pathname, sanitized_pathname);

	check_parameters_properties(sanitized_pathname, __func__);

	int execv_result = execv_wrapper(pathname, argv);

	// If execv succeeds this code will never be executed

	if(execv_result == -1){
		zlogf_time(ZLOG_INFO_LOG_MSG, "[!] EXECV ERROR: %s\n", strerror(errno));
	}

	return execv_result;
}

int execvp(const char *file, char *const argv[]){

	const char *sanitized_file = sanitize_and_get_absolute_path(file);

	print_function_and_path(__func__, file, sanitized_file);

	check_parameters_properties(sanitized_file, __func__);

	int execvp_result = execvp_wrapper(file, argv);

	// If execv succeeds this code will never be executed

	if(execvp_result == -1){
		zlogf_time(ZLOG_INFO_LOG_MSG, "[!] EXECVP ERROR: %s\n", strerror(errno));
	}

	return execvp_result;

}

int execve(const char *pathname, char *const argv[], char *const envp[]){

	const char *sanitized_pathname = sanitize_and_get_absolute_path(pathname);

	print_function_and_path(__func__, pathname, sanitized_pathname);

	check_parameters_properties(sanitized_pathname, __func__);

	int execve_result = execve_wrapper(pathname, argv, envp);

	// If execv succeeds this code will never be executed

	if(execve_result == -1){
		zlogf_time(ZLOG_INFO_LOG_MSG, "[!] EXECVE ERROR: %s\n", strerror(errno));
	}

	return execve_result;

}

int execvpe(const char *file, char *const argv[], char *const envp[]){

	const char *sanitized_file = sanitize_and_get_absolute_path(file);

	print_function_and_path(__func__, file, sanitized_file);

	check_parameters_properties(sanitized_file, __func__);

	int execvpe_result = execvpe_wrapper(file, argv, envp);

	// If execv succeeds this code will never be executed

	if(execvpe_result == -1){
		zlogf_time(ZLOG_INFO_LOG_MSG, "[!] EXECVPE ERROR: %s\n", strerror(errno));
	}

	return execvpe_result;

}

FILE * popen(const char *command, const char *type){

	print_function_and_path(__func__, command, command);

	check_parameters_properties(command, __func__);

	if(original_popen == NULL){
		original_popen = dlsym_wrapper(__func__);
	}

	FILE *file = original_popen(command, type);

	if(file == NULL){
		zlogf_time(ZLOG_INFO_LOG_MSG, "[!] POPEN ERROR: %s\n", strerror(errno));
	}

	return file;

}

int mount(const char *source, const char *target, const char *filesystemtype, unsigned long mountflags, const void *data){

	const char *sanitized_source = sanitize_and_get_absolute_path(source);
	
	print_function_and_path(__func__, source, sanitized_source);

	check_parameters_properties(sanitized_source, __func__);

	if(original_mount == NULL){
		original_mount = dlsym_wrapper(__func__);
	}

	int mount_result = original_mount(source, target, filesystemtype, mountflags, data);

	if(mount_result == -1){
		zlogf_time(ZLOG_INFO_LOG_MSG, "[!] MOUNT ERROR: %s\n", strerror(errno));
	}

	return mount_result;

}

//#########################
