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

#include <limits.h>
#include <time.h>
#include <utime.h>
#include <sys/time.h>

#include <sys/mman.h>

////TODO implement logger and replace printf family with corresponding log level

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
static FILE *(*original_fdopen)(int fd, const char *mode) = NULL;
static int (*original_mknod)(const char *path, mode_t mode, dev_t dev) = NULL;
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

// execve, execl, execle, execlp, execv, execve, execvp

// doubts
static FILE *(*original_popen)(const char *command, const char *type) = NULL;
static int (*original_pclose)(FILE *stream) = NULL;
static int (*original_mount)(const char *source, const char *target, const char *filesystemtype, unsigned long mountflags, const void *data) = NULL;











/// ########## Hooked functions ##########

/// <-------------------------------------------------> 

/// ########## Prototype declaration ##########

void check_parameters_properties(const char *, const char *);
void* dlsym_wrapper(const char *);
int open_wrapper(const char *, int, va_list);
int openat_wrapper(int, const char *, int, va_list);
int chdir_wrapper(const char *);
ino_t get_inode(const char *);
const char * sanitize_and_get_absolute_path(const char *);
const char * sanitize_path(const char *);
const char * sanitize_relative_path(const char *);
const char * get_file_path_from_directory_fd(const char*, int);
const char * sanitize_and_get_absolute_path_from_dir_file_descriptor(const char *, int);
void timestamp();
int file_does_exist(const char *);
char * get_directory_from_fd(int);
bool path_is_absolute(const char *);
/// ########## Prototype declaration ##########

/// <-------------------------------------------------> 

/// ########## file_objects_info.c ##########
typedef struct{
	char *path;
	ino_t inode;
} file_object_info;

typedef struct{
	file_object_info *list;
	size_t used;
	size_t size;
} file_objects_info;

// -- Array operations -- //
void initialize_array(file_objects_info *, size_t);
void upsert_inode_in_array(file_objects_info *, const char *, ino_t);
void upsert_path_in_array(const char *, const char *);
void free_array(file_objects_info *);
int find_index_in_array(file_objects_info *, const char *);
file_object_info get_from_array_at_index(file_objects_info *, int);
void remove_from_array_at_index(file_objects_info *, int);
void print_contents_of_array(file_objects_info *);


/// ########## file_objects_info.c ##########

/// <-------------------------------------------------> 

/// ########## GLOBAL VARIABLES ##########
extern char *program_invocation_name;
file_objects_info g_array;
/// ########## GLOBAL VARIABLES ##########

/// <-------------------------------------------------> 

/// ########## Array management ##########
void check_dlsym_error(){
	char * error = dlerror();
	if(error != NULL){
		//printf("There were errors while retrieving the original function from the dynamic linker/loader.\nDlsym error: %s\n", error);
		exit(EXIT_FAILURE);
	}
}


/*
    Initializes the given array with the given size, allocating
    the corresponding memory.
*/
void initialize_array(file_objects_info *array, size_t size){
	////printf("Initialize has been called for array: %X and size: %lu\n", &(*array), size);
	//printf("Initialize array has been called\n");
	array->list = (file_object_info *) calloc(size, sizeof(file_object_info)); 
	if(!array->list){
		//printf("Error allocating memory for array in Initialize process.\n");
		exit(EXIT_FAILURE);
	}
	array->used = 0;
	array->size = size;

    //Elements of array are contiguous
    //memset(&array->list[array->used], 0, sizeof(file_object_info) * initialSize);
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
				printf("Error trying to realloc size for array in Insert process.\n");
				exit(EXIT_FAILURE);
			} else {
				array->list = aux;
			}

        //Initializing new elements of realocated array
			memset(&array->list[array->used], 0, sizeof(file_object_info) * (array->size - array->used));

		}
		array->list[array->used].path = strdup(path);
		array->list[array->used].inode = inode;
		array->used++;
	}
}

/*
    Frees the memory used by the given array. This function
    is ment to be called at the end of the program.
*/
void free_array(file_objects_info *array){

	print_contents_of_array(array);
	fflush(stdout);

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
	//printf("Invoked find index in array\n");
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

	//printf("Called remove_from_array_at_index with index %d\n", index);
	//print_contents_of_array(&g_array);

	int number_elements = array->used;
	if(index < number_elements){

		for(int i = index; i < number_elements; i++){
			array->list[i] = array->list[i+1];
		}

	} 

	array->used--;
}

void print_contents_of_array(file_objects_info *array){

	printf("Array used: %lu\nArray size: %lu\n", array->used, array->size);

	for(uint i = 0; i < array->used; i++){
		printf("[+] Element at position %d: path-> %s inode ->%lu\n", i, array->list[i].path, array->list[i].inode);
	}
}

/*
	Upsert is update + insert. If the element already exists, update it. 
	Otherwise insert it.
*/
void upsert_path_in_array(const char *oldpath, const char *newpath){
	
	int index = find_index_in_array(&g_array, oldpath);
	if(index == -1){
		ino_t inode = get_inode(newpath);
		upsert_inode_in_array(&g_array, newpath, inode);
	} else {
		g_array.list[index].path = strdup(newpath);
	}

}

/// ########## Array management ##########

/// <-------------------------------------------------> 

/// ########## Core and useful functions ##########
void print_function_and_path(const char* func, const char* path){
	printf("User invoked %s via process %s on: %s\n", func, program_invocation_name, path);
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

	path = sanitize_and_get_absolute_path(path);

	print_function_and_path(caller_function_name, path);

	if(file_does_exist(path)){
		ino_t inode = get_inode(path);

		int index = find_index_in_array(&g_array, path);
	
		if(index < 0){
			upsert_inode_in_array(&g_array, path, inode);
		} else {
			file_object_info aux = get_from_array_at_index(&g_array,index);
			if(aux.inode != inode){
			//printf("FILE %s EXISTS: %d\n", path, exists);
				timestamp();
				printf("[+][!] WARNING! TOCTTOU DETECTED!. Inode of <%s> has changed since it was previously invoked. Threat detected when invoking <%s> function. It was previously <%lu> and now it is <%lu>. \n [#] PROGRAM ABORTED [#]\n", path, caller_function_name, aux.inode, inode);
				fflush(stdout);
				exit(EXIT_FAILURE);
			} else {
				// #### EMPTY BLOCK TODO!
				//printf("NODES ARE EQUAL!!! :) HAPPINESS");
			}
		}
	} else { // if file_does_exist
		//printf("File %s does not exist\n", path);
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

/*
    Retrieves the corresponding inode of a given path. If path is a symlink
    it retrieves the inode of the target rather than the symlink itself. 
*/
ino_t get_inode(const char *path){
	int fd;
	ino_t inode;
	//printf("User invoked get_inode for %s\n", path);
    // Parenthesis are needed because of operator precedence.
    // https://en.cppreference.com/w/c/language/operator_precedence

    // ERRORS CHECKiNG
    /*
	if((fd = open_wrapper(path, O_RDONLY)) < 0){
		//printf("Errors occurred while trying to access %s.\nAborting.", path);
		perror("Error1 is: ");
		fflush(stdout);
        //exit(EXIT_FAILURE);
	} else {
		//printf("Created fileDescriptor is: %d\n", fd);
		struct stat file_stat;
		if((ret = fstat(fd, &file_stat)) < 0 ){
			//printf("Errors occurred while trying to stat %d file descriptor.\nAborting.", fd);
			perror("Error2 is: ");
			close(fd);
            //exit(EXIT_FAILURE);
		} else {
			inode = file_stat.st_ino;
            //After opening a FD, it must be closed
			close(fd);
		}
	}*/

	fd = open_wrapper(path, O_RDONLY, NULL);
	if (fd < 0){
		printf("Errors occurred while getting inode of %s\n", path);
		printf("ERROR: %s\n", strerror(errno));
	}
	struct stat file_stat;
	fstat(fd, &file_stat);
	inode = file_stat.st_ino;
	close(fd);

	////printf("User invoked get_inode for %s and it's %lu\n", path, inode);
	//printf("User invoked get_inode for %s \n", path);
	return inode;
}

/*
	Function to get full path of a given parameter without resolving, expanding
	symbolic links. That's why realpath() is useless. 
	https://stackoverflow.com/questions/4774116/realpath-without-resolving-symlinks/34202207#34202207
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
	symbolic links but using a directory file descriptor as current working dir. 
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
	Function to sanitize the given path.
	Based on: https://stackoverflow.com/questions/4774116/realpath-without-resolving-symlinks/34202207#34202207
*/
const char * sanitize_path(const char *src) {

		size_t res_len = 0;
        size_t src_len = strlen(src);
		char *res = malloc(src_len + 1);
        
        const char *pointer;
        const char *end_pointer = &src[src_len];
        const char *next_pointer;

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
	Function used to determine whether a path is absolute. If it isn't it's, 
	obviosuly, because it is relative.
*/
bool path_is_absolute(const char *path){
	return (path[0] == '/');
}

/*
	Function used to sanitize a relative path. That is, sanitize an absoulte path
	but without the first '/' slash character. That's why this function is as 
	simple as calling sanitize_path and then simply make use of pointer arithmetics
	in order to chop the sanitized path. If char *src = "/file" after src++ it will
	become src = "file".
*/
const char * sanitize_relative_path(const char *src){
	bool absolute = path_is_absolute(src);

	src = sanitize_path(src);
	if(absolute){
		return src;
	} else {
		return ++src;
	}
}

/*
	Function used to print current time with the following format:
	DD mm dd hh:MM:ss yyyy [hh:MM:ss.nanoseconds]
	Example: <Wed May  1 13:44:17 2019
 				[13:45:17.676232682]>
*/
void timestamp(){
	time_t ltime;
	ltime = time(NULL);

	struct tm *tm_struct = localtime(&ltime);

	struct timespec spec;
	clock_gettime(CLOCK_REALTIME, &spec);

	printf("<%s [%d:%d:%d.%lu]>\n", asctime(tm_struct), tm_struct->tm_hour, tm_struct->tm_min + 1, tm_struct->tm_sec, spec.tv_nsec);

}

int file_does_exist(const char *pathname){
	if(!strcmp(pathname, "/dev/tty")){
		return 1;
	} else {
		if(open_wrapper(pathname, O_RDONLY, NULL) < 0){
			return 0;
		} else {
			return 1;
		}
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

/*
	Function used to retrieve as string the full path to a file given a
	file descriptor pointing to a directory. It is assumed that the files 
	is indeed within that directory. The file translates the file descriptor
	into the actual directory (string).
*/
const char * get_file_path_from_directory_fd(const char *path, int dirfd){
	
	/*
	char *directory_fd_path = get_directory_from_fd(dirfd);

   	// +2 for null-trailing byte and the "/" slash between them
   	int path_length = strlen(directory_fd_path) + strlen(path) + 2;
   	
   	char aux_path[path_length];

   	snprintf(aux_path, path_length, "%s/%s", directory_fd_path, path);

   	free(directory_fd_path);

   	return sanitize_and_get_absolute_path(aux_path);

	*/

	return sanitize_and_get_absolute_path_from_dir_file_descriptor(path, dirfd);

  }

/// ########## Core and useful functions ##########

/// <-------------------------------------------------> 

/// ########## Coconstructor and Destructor ##########
static void before_main(void) __attribute__((constructor));
static void after_main(void) __attribute__((destructor));

static void before_main(void){
	printf("######### BEFORE MAIN!!!!\n [+] I AM %s WITH PID %d and PPID %d [+]\n", program_invocation_name, getpid(), getppid());
}

static void after_main(void){
	//printf("######### AFTER MAIN!!!!\n");

	printf("##### AFTER MAIN\n I AM  %s PID: %d PPID: %d\n", program_invocation_name, getpid(), getppid());
	printf("g_array used: %lu size: %lu\n", g_array.used, g_array.size);

	free_array(&g_array);
	//printf("Dirección de array: %X\n", &g_array);

}
/// ########## Coconstructor and Destructor ##########

/// <-------------------------------------------------> 

/// ########## Hooked functions replacement code ##########

int __xstat(int ver, const char *path, struct stat *buf)
{
	//printf("I'VE RECEIVED PATH %s\n", path);

	path = sanitize_and_get_absolute_path(path);

	print_function_and_path(__func__, path);

	upsert_inode_in_array(&g_array, path, get_inode(path));

	if ( original_xstat == NULL ) {
		original_xstat = dlsym_wrapper(__func__);
	}

  ////printf("xstat64 %s\n",path);
	return original_xstat(ver, path, buf);
} 

int __xstat64(int ver, const char *path, struct stat64 *buf)
{

	path = sanitize_and_get_absolute_path(path);

	print_function_and_path(__func__, path);

	upsert_inode_in_array(&g_array, path, get_inode(path));

	if ( original_xstat64 == NULL ) {
		original_xstat64 = dlsym_wrapper(__func__);
	}

  ////printf("xstat64 %s\n",path);
	return original_xstat64(ver, path, buf);
}

int __lxstat64(int ver, const char *path, struct stat64 *buf)
{

	path = sanitize_and_get_absolute_path(path);

	print_function_and_path(__func__, path);

	upsert_inode_in_array(&g_array, path, get_inode(path));

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

	path = sanitize_and_get_absolute_path(path);

	bool path_exists_before = file_does_exist(path);
	struct stat new_file;

	check_parameters_properties(path, __func__);

	va_list variable_arguments;
	va_start(variable_arguments, flags);

	int open_result = open_wrapper(path, flags, variable_arguments);

	va_end(variable_arguments);

	/*
		open(), openat(), and creat() return the new file descriptor, or -1
		if an error occurred (in which case, errno is set appropriately).
	*/

	if(open_result == -1){
		printf("OPEN ERROR: %s\n", strerror(errno));
	} else {
		/*
		If file didn't exist before actual open call (because otherwise it'd
		have been treated in check_parameters_properties) and fstat returns
		zero (success) a new file has been created.
		*/
		if(!path_exists_before && !fstat(open_result, &new_file)){
			/*
				New file has been just created. Now there are two options:
					- There is already an entry in the array referencing the path
						so only the inode must be updated.
					- There is no entry in the array so just insert.
				Both actions are carreid out by upsert_inode_in_array
			*/
			
			ino_t inode = get_inode(path);

			upsert_inode_in_array(&g_array, path, inode);
			
		}
	}

	return open_result;
}

/*
	open64() behaves exactly in the same way as open(). Please
	read open() docs.
*/
int open64(const char *path, int flags, ...)
{

	path = sanitize_and_get_absolute_path(path);

	bool path_exists_before = file_does_exist(path);
	struct stat new_file;

	check_parameters_properties(path, __func__);

	va_list variable_arguments;
	va_start(variable_arguments, flags);

	int open64_result = open64_wrapper(path, flags, variable_arguments);

	va_end(variable_arguments);

	/*
		open(), openat(), and creat() return the new file descriptor, or -1
		if an error occurred (in which case, errno is set appropriately).
	*/

	if(open64_result == -1){
		printf("OPEN64 ERROR: %s\n", strerror(errno));
	} else {

		if(!path_exists_before && !fstat(open64_result, &new_file)){
			
			ino_t inode = get_inode(path);

			upsert_inode_in_array(&g_array, path, inode);
			
		}
	}

	return open64_result;
}

int access(const char *path, int mode){

	path = sanitize_and_get_absolute_path(path);

	print_function_and_path(__func__, path);

	upsert_inode_in_array(&g_array, path, get_inode(path));

	if(original_access == NULL){
		original_access = dlsym_wrapper(__func__);
	}	

	return original_access(path, mode);

}

FILE *fopen(const char *path, const char *mode){

	path = sanitize_and_get_absolute_path(path);

	check_parameters_properties(path, __func__);

	if(original_fopen == NULL){
		original_fopen = dlsym_wrapper(__func__);
	}

	return original_fopen(path, mode);

}

int unlink(const char *path){

	printf("Process %s with pid %d called %s for path %s\n", program_invocation_name, getpid(), __func__, path);

	if(original_unlink == NULL){
		original_unlink = dlsym_wrapper(__func__);
	}
	
	path = sanitize_and_get_absolute_path(path);

   	print_function_and_path(__func__, path);

	int unlink_result = original_unlink(path);

	/*
		On success, zero is returned. On error, -1 is returned, and errno is
		set appropriately.
	*/
	if(unlink_result == -1){
		printf("UNLINK ERROR: %s\n", strerror(errno));
	} else {
		int index = find_index_in_array(&g_array, path);
		if(index >= 0){
			remove_from_array_at_index(&g_array, index);
			//g_array.list[index].inode = -1;
		}
	}

	return unlink_result;

}

int unlinkat(int dirfd, const char *path, int flags){
	printf("Process %s with pid %d called %s for path %s\n", program_invocation_name, getpid(), __func__, path);

	if(original_unlinkat == NULL){
		original_unlinkat = dlsym_wrapper(__func__);
	}
	
	const char *full_path;
	if(path_is_absolute(path)){
		full_path = sanitize_and_get_absolute_path(path);
	} else {
		full_path = get_file_path_from_directory_fd(path, dirfd);
	}

	print_function_and_path(__func__, full_path);

	/*
		Note that the original function gets passed only path, not full_path
		since the retrieval of full_path is a mere operation in order to
		keep g_array updated.
	*/
   	int unlinkat_result = original_unlinkat(dirfd, path, flags);

   	/*
		On success, unlinkat() returns 0. On error, -1 is returned and errno
		is set to indicate the error.
   	*/
   	if(unlinkat_result == -1){
		printf("UNLINKAT ERROR: %s\n", strerror(errno));
	} else {

	   	int index = find_index_in_array(&g_array, full_path);

		if(index >= 0){
			remove_from_array_at_index(&g_array, index);
			//g_array.list[index].inode = -1;
		}
	}

	return unlinkat_result;

}

int openat(int dirfd, const char *path, int flags, ...){
	printf("Process %s with pid %d called %s for path %s\n", program_invocation_name, getpid(), __func__, path);

	const char *full_path;
	if(path_is_absolute(path)){
		full_path = sanitize_and_get_absolute_path(path);
	} else {
		full_path = get_file_path_from_directory_fd(path, dirfd);
	}

	bool path_exists_before = file_does_exist(full_path);
	struct stat new_file;

	check_parameters_properties(full_path, __func__);

	va_list variable_arguments;
	va_start(variable_arguments, flags);

	int openat_result =  openat_wrapper(dirfd, path, flags, variable_arguments);

	va_end(variable_arguments);

	/*
		On success, openat() returns a new file descriptor. On error, -1 is
		returned and errno is set to indicate the error.
	*/
	if(openat_result == -1){
		printf("OPENAT ERROR: %s\n", strerror(errno));
	} else {

		if(!path_exists_before && !fstat(openat_result, &new_file)){

			ino_t inode = get_inode(full_path);
			
			upsert_inode_in_array(&g_array, full_path, inode);
			

		}
	}
	return openat_result;

}


/*
	Creates a symbolic link called newpath that poins to oldpath.
*/
int symlink(const char *oldpath, const char *newpath){
    printf("Process %s with pid %d called %s for oldpath: %s and newpath: %s\n", program_invocation_name, getpid(), __func__, oldpath, newpath);

    if(original_symlink == NULL){
    	original_symlink = dlsym_wrapper(__func__);
    }
	
    newpath = sanitize_and_get_absolute_path(newpath);

   	print_function_and_path(__func__, newpath);

    int symlink_result = original_symlink(oldpath, newpath);

    /*
		Upon successful completion, symlink() shall return 0; otherwise, it 
		shall return -1 and set errno to indicate the error.
    */
    if(symlink_result == -1){
		printf("SYMLINK ERROR: %s\n", strerror(errno));
	} else {

	    ino_t inode = get_inode(newpath);

		upsert_inode_in_array(&g_array, newpath, inode);
		
	}

	return symlink_result;

}

/*
	Creates a symbolic link to oldpath called newpath in the directory pointed to by newdirfd.
*/
int symlinkat(const char *oldpath, int newdirfd, const char *newpath){
	printf("Process %s with pid %d called %s for oldpath: %s and newpath: %s\n", program_invocation_name, getpid(), __func__, oldpath, newpath);

	if(original_symlinkat == NULL){
		original_symlinkat = dlsym_wrapper(__func__);
	}
	
	const char *full_path;
	if(path_is_absolute(newpath)){
		full_path = sanitize_and_get_absolute_path(newpath);
	} else {
		full_path = get_file_path_from_directory_fd(newpath, newdirfd);
	}

   	print_function_and_path(__func__, full_path);

	int symlinkat_result = original_symlinkat(oldpath, newdirfd, newpath);

	/*
		On success, symlinkat() returns 0. On error, -1 is returned and errno
		is set to indicate the error.
	*/
	if(symlinkat_result == -1){
		printf("SYMLINKAT ERROR: %s\n", strerror(errno));
	} else {
		ino_t inode = get_inode(full_path);
		upsert_inode_in_array(&g_array, full_path, inode);
	}

	print_contents_of_array(&g_array);

	return symlinkat_result;

}

/*
	remove() deletes a name from the file system. 
	It calls unlink(2) for files, and rmdir(2) for directories.	
*/
int remove(const char *path) {

	printf("Program %s with PID: %d called remove for path: %s\n", program_invocation_name, getpid(), path);
	
	if(original_remove == NULL){
    	original_remove = dlsym_wrapper(__func__);
    }
    
    path = sanitize_and_get_absolute_path(path);

	print_function_and_path(__func__, path);

	int remove_result = original_remove(path);

	/*
		On success, zero is returned. On error, -1 is returned, and errno is
		set appropriately.
	*/
	if(remove_result == -1){
		printf("REMOVE ERROR: %s\n", strerror(errno));
	} else {
		int index = find_index_in_array(&g_array, path);

		if(index >= 0){
			remove_from_array_at_index(&g_array, index);
			//g_array.list[index].inode = -1;
		}
	}

    return remove_result;

}

/*
	The system call mknod() creates a filesystem node (file, device
    special file, or named pipe) named pathname, with attributes
    specified by mode and dev.
*/
int mknod(const char *path, mode_t mode, dev_t dev){
    printf("Process %s with pid %d called %s for path %s\n", program_invocation_name, getpid(), __func__, path);

    if(original_mknod == NULL){
    	original_mknod = dlsym_wrapper(__func__);
    }
	
    path = sanitize_and_get_absolute_path(path);

   	print_function_and_path(__func__, path);

    int mknod_result = original_mknod(path, mode, dev);

    /*
		mknod() and mknodat() return zero on success, or -1 if an error
		occurred (in which case, errno is set appropriately).
    */

    if(mknod_result == -1){
		printf("MKNOD ERROR: %s\n", strerror(errno));
	} else {

	    ino_t inode = get_inode(path);

	    upsert_inode_in_array(&g_array, path, inode);
	    
	}

    return mknod_result;

}

/*
	The system call mknod() creates a filesystem node (file, device
    special file, or named pipe) named pathname, with attributes
    specified by mode and dev.
*/
int __xmknod(int ver, const char *path, mode_t mode, dev_t *dev){
    printf("Process %s with pid %d called %s for path %s\n", program_invocation_name, getpid(), __func__, path);

    if(original_xmknod == NULL){
    	original_xmknod = dlsym_wrapper(__func__);
    }
	
    path = sanitize_and_get_absolute_path(path);

   	print_function_and_path(__func__, path);

    int mknod_result = original_xmknod(ver, path, mode, dev);

    /*
		mknod() and mknodat() return zero on success, or -1 if an error
		occurred (in which case, errno is set appropriately).
    */
    if(mknod_result == -1){
		printf("MKNOD ERROR: %s\n", strerror(errno));
	} else {

	    ino_t inode = get_inode(path);

	    upsert_inode_in_array(&g_array, path, inode);
	    
	}

    return mknod_result;

}

/*
	The mknodat() system call operates in exactly the same way as mknod(2), 
	except that it creates a speial or ordinary file relative to a 
	directory file descriptor.
*/
int __xmknodat(int ver, int dirfd, const char *path, mode_t mode, dev_t *dev){
	printf("Process %s with pid %d called %s for path %s\n", program_invocation_name, getpid(), __func__, path);

	if(original_xmknodat == NULL){
		original_xmknodat = dlsym_wrapper(__func__);
	}
	
	const char *full_path;
	if(path_is_absolute(path)){
		full_path = sanitize_and_get_absolute_path(path);
	} else {
		full_path = get_file_path_from_directory_fd(path, dirfd);
	}

   	print_function_and_path(__func__, full_path);

	int mknodat_result = original_xmknodat(ver, dirfd, path, mode, dev);

	/*
		On success, mknodat() returns 0. On error, -1 is returned and errno is
		set to indicate the error.
	*/
	if(mknodat_result == -1){
		printf("MKNODAT ERROR: %s\n", strerror(errno));
	} else {
		ino_t inode = get_inode(full_path);
    	upsert_inode_in_array(&g_array, full_path, inode);
    }

    return mknodat_result;
}


/*
	Creates a new hardlink called newpath that points to oldpath. 
*/
int link(const char *oldpath, const char *newpath){
   printf("Process %s with pid %d called %s for oldpath: %s and newpath: %s\n", program_invocation_name, getpid(), __func__, oldpath, newpath);

   if(original_link == NULL){
   		original_link = dlsym_wrapper(__func__);
   }

	newpath = sanitize_and_get_absolute_path(newpath);

   	print_function_and_path(__func__, newpath);

   	int link_result = original_link(oldpath, newpath);

   	/*
		Upon successful completion, 0 shall be returned. Otherwise, -1 shall
		be returned and errno set to indicate the error.
   	*/
   	if(link_result == -1){
		printf("LINK ERROR: %s\n", strerror(errno));
	} else {

	   	ino_t inode = get_inode(newpath);

		upsert_inode_in_array(&g_array, newpath, inode);
	
	}

   return link_result;

}


/*
	Creates a new hardlink called newpath in directory pointed to by newdirfd. The hardlink newpath
	points to file oldpath, which is located in directroy pointed to by olddirfd. 
*/
int linkat(int olddirfd, const  char *oldpath, int newdirfd, const char *newpath, int flags){
	printf("Process %s with pid %d called %s for oldpath: %s and newpath: %s\n", program_invocation_name, getpid(), __func__, oldpath, newpath);

	if(original_linkat == NULL){
		original_linkat = dlsym_wrapper(__func__);
	}

	const char *full_path;
	if(path_is_absolute(newpath)){
		full_path = sanitize_and_get_absolute_path(newpath);
	} else {
		full_path = get_file_path_from_directory_fd(newpath, newdirfd);
	}

    print_function_and_path(__func__, full_path);

    int linkat_result = original_linkat(olddirfd, oldpath, newdirfd, newpath, flags);

    /*
		On success, linkat() returns 0. On error, -1 is returned and errno is
		set to indicate the error.
    */
    if(linkat_result == -1){
    	printf("LINKAT ERROR: %s\n", strerror(errno));
    } else {

    	ino_t inode = get_inode(full_path);
		upsert_inode_in_array(&g_array, full_path, inode);

    }

    return linkat_result;

}

int creat64(const char *path, mode_t mode){
    printf("Process %s with pid %d called %s for path %s\n", program_invocation_name, getpid(), __func__, path);

    if(original_creat64 == NULL){
    	original_creat64 = dlsym_wrapper(__func__);
    }

    path = sanitize_and_get_absolute_path(path);

    print_function_and_path(__func__, path);

    int creat64_result = original_creat64(path, mode);

    /*
		If successful, creat() and creat64() return a nonnegative integer, 
		which is the lowest numbered unused valid file descriptor. On failure,
		they return -1, do not create or modify any files, and set errno to 
		one of the following values:
    */
    if(creat64_result == -1){
    	printf("CREAT64 ERROR: %s\n", strerror(errno));
    } else {;

	    ino_t inode = get_inode(path);

		upsert_inode_in_array(&g_array, path, inode);
    }

    return creat64_result;

}

int creat(const char *path, mode_t mode){

	printf("Program %s with PID: %d called creat for path: %s", program_invocation_name, getpid(), path);
    
    if(original_creat == NULL){
    	original_creat = dlsym_wrapper(__func__);
    }

    path = sanitize_and_get_absolute_path(path);

    print_function_and_path(__func__, path);

    int creat_result = original_creat(path, mode);

     /*
		If successful, creat() and creat64() return a nonnegative integer, 
		which is the lowest numbered unused valid file descriptor. On failure,
		they return -1, do not create or modify any files, and set errno to 
		one of the following values:
    */
    if(creat_result == -1){
    	printf("CREAT ERROR: %s\n", strerror(errno));
    } else {

	    ino_t inode = get_inode(path);

		upsert_inode_in_array(&g_array, path, inode);

    }

    return creat_result;

}

int rmdir(const char *path){

	if(original_rmdir == NULL){
    	original_rmdir = dlsym_wrapper(__func__);
    }
    
    path = sanitize_and_get_absolute_path(path);

	print_function_and_path(__func__, path);

	int rmdir_result = original_rmdir(path);

	/*
		Upon successful completion, the function rmdir() shall return 0. 
		Otherwise, -1 shall be returned, and errno set to indicate the error. 
		If -1 is returned, the named directory shall not be changed.
	*/
	if(rmdir_result == -1){
		printf("RMDIR ERROR: %s\n", strerror(errno));
	} else {
		int index = find_index_in_array(&g_array, path);

		if(index >= 0){
			remove_from_array_at_index(&g_array, index);
			//g_array.list[index].inode = -1;
		}
	}

    return rmdir_result;

}

ssize_t readlink(const char *pathname, char *buf, size_t bufsiz){

	pathname = sanitize_and_get_absolute_path(pathname);

	print_function_and_path(__func__, pathname);

	upsert_inode_in_array(&g_array, pathname, get_inode(pathname));

	if ( original_readlink == NULL ) {
		original_readlink = dlsym_wrapper(__func__);
	}

	return original_readlink(pathname, buf, bufsiz);
}

ssize_t readlinkat(int dirfd, const char *pathname, char *buf, size_t bufsiz){


	if(original_readlinkat == NULL){
		original_readlinkat = dlsym_wrapper(__func__);
	}

	const char *full_path;
	if(path_is_absolute(pathname)){
		full_path = sanitize_and_get_absolute_path(pathname);
	} else {
		full_path = get_file_path_from_directory_fd(pathname, dirfd);
	}

    print_function_and_path(__func__, full_path);

    int readlinkat_result = original_readlinkat(dirfd, pathname, buf, bufsiz);

    /*
		On success, these calls return the number of bytes placed in buf.
       	(If the returned value equals bufsiz, then truncation may have
       	occurred.)  On error, -1 is returned and errno is set to indicate the
       	error.
    */
    if(readlinkat_result == -1){
    	printf("READLINKAT ERROR: %s\n", strerror(errno));
    } else {

    	ino_t inode = get_inode(full_path);
		upsert_inode_in_array(&g_array, full_path, inode);

    }

    return readlinkat_result;
}

int rename(const char *oldpath, const char *newpath){

	const char *full_oldpath = sanitize_and_get_absolute_path(oldpath);
	const char *full_newpath = sanitize_and_get_absolute_path(newpath);

	print_function_and_path(__func__, full_oldpath);

	if(original_rename == NULL){
		original_rename = dlsym_wrapper(__func__);
	}

	int rename_result = original_rename(oldpath, newpath);

	/*
		On success, zero is returned.  On error, -1 is returned, and errno is
		set appropriately.
	*/
	if( rename_result == -1){
		printf("RENAME ERROR: %s\n", strerror(errno));
	} else {
		upsert_path_in_array(full_oldpath, full_newpath);
	}

	return rename_result;
}

int renameat(int olddirfd, const char *oldpath, int newdirfd, const char *newpath){

	if(original_renameat == NULL){
		original_renameat = dlsym_wrapper(__func__);
	}

	const char *full_old_path;
	const char *full_new_path;

	if(path_is_absolute(oldpath)){
		full_old_path = sanitize_and_get_absolute_path(oldpath);
	} else {
		full_old_path = get_file_path_from_directory_fd(oldpath, olddirfd);
	}	

	if(path_is_absolute(newpath)){
		full_new_path = sanitize_and_get_absolute_path(newpath);
	} else {
		full_new_path = get_file_path_from_directory_fd(newpath, newdirfd);
	}

    print_function_and_path(__func__, full_old_path);

    int renameat_result = original_renameat(olddirfd, oldpath, newdirfd, newpath);

    /*
		On success, zero is returned.  On error, -1 is returned, and errno is 
		set appropriately.
    */
    if(renameat_result == -1){
    	printf("RENAMEAT ERROR: %s\n", strerror(errno));
    } else {
    	upsert_path_in_array(full_old_path, full_new_path);
    }

    return renameat_result;
}

FILE *fopen64(const char *path, const char *mode){

	path = sanitize_and_get_absolute_path(path);

	check_parameters_properties(path, __func__);

	if(original_fopen64 == NULL){
		original_fopen64 = dlsym_wrapper(__func__);
	}

	return original_fopen64(path, mode);

}

int mkfifo(const char *pathname, mode_t mode){

	if(original_mkfifo == NULL){
    	original_mkfifo = dlsym_wrapper(__func__);
    }
	
    path = sanitize_and_get_absolute_path(pathname);

   	print_function_and_path(__func__, path);

    int mkfifo_result = original_mkfifo(path, mode);

    /*
		On success mkfifo() and mkfifoat() return 0.  In the case of an
    	error, -1 is returned (in which case, errno is set appropriately).
    */
    if(mkfifo_result == -1){
		printf("MKFIFO ERROR: %s\n", strerror(errno));
	} else {

	    ino_t inode = get_inode(path);

	    upsert_inode_in_array(&g_array, path, inode);
	}

    return mkfifo_result;
}

int mkfifoat(int dirfd, const char *pathname, mode_t mode){
	if(original_mkfifoat == NULL){
		original_mkfifoat = dlsym_wrapper(__func__);
	}

	const char *full_path;
	if(path_is_absolute(pathname)){
		full_path = sanitize_and_get_absolute_path(pathname);
	} else {
		full_path = get_file_path_from_directory_fd(pathname, dirfd);
	}

    print_function_and_path(__func__, full_path);

    int mkfifoat_result = original_mkfifoat(dirfd, pathname, buf, bufsiz);

    /*
		On success, these calls return the number of bytes placed in buf.
       	(If the returned value equals bufsiz, then truncation may have
       	occurred.)  On error, -1 is returned and errno is set to indicate the
       	error.
    */
    if(mkfifoat_result == -1){
    	printf("READLINKAT ERROR: %s\n", strerror(errno));
    } else {

    	ino_t inode = get_inode(full_path);
		upsert_inode_in_array(&g_array, full_path, inode);

    }

    return mkfifoat_result;
}
//#########################
/*
int
creat64(const char *pathname, mode_t mode)
{
    intercept("creat64", 2);
    set_errno();
    return -1;
}

int
creat(const char *pathname, mode_t mode)
{
    intercept("creat", 2);
    set_errno();
    return -1;
}

int
close(int fd)
{
    intercept("close", 2);
    set_errno();
    return -1;
}

int
open64(const char *pathname, int flags, ...)
{
    intercept("open64", 2);
    set_errno();
    return -1;
}



ssize_t
read(int fd, void *buf, size_t count)
{
    intercept("read", 2);
    set_errno();
    return -1;
}

ssize_t
readv(int fd, const struct iovec *vector, int count)
{
    intercept("readv", 2);
    set_errno();
    return -1;
}

ssize_t
pread(int fd, void *buf, size_t count, unsigned long offset)
{
    intercept("pread", 2);
    set_errno();
    return -1;
}

ssize_t
pread64(int fd, void *buf, size_t count, uint64_t offset)
{
    intercept("pread64", 2);
    set_errno();
    return -1;
}

ssize_t
write(int fd, const void *buf, size_t count)
{
    intercept("write", 2);
    set_errno();
    return -1;
}

ssize_t
writev(int fd, const struct iovec *vector, int count)
{
    intercept("writev", 2);
    set_errno();
    return -1;
}

ssize_t
pwrite(int fd, const void *buf, size_t count, unsigned long offset)
{
    intercept("pwrite", 2);
    set_errno();
    return -1;
}

ssize_t
pwrite64(int fd, const void *buf, size_t count, uint64_t offset)
{
    intercept("pwrite64", 2);
    set_errno();
    return -1;
}

off_t
lseek(int fildes, unsigned long offset, int whence)
{
    intercept("lseek", 2);
    set_errno();
    return -1;
}

off_t
lseek64(int fildes, uint64_t offset, int whence)
{
    intercept("lseek64", 2);
    set_errno();
    return -1;
}

int
dup(int fd)
{
    intercept("dup", 2);
    set_errno();
    return -1;
}

int
dup2(int oldfd, int newfd)
{
    intercept("dup2", 2);
    set_errno();
    return -1;
}

int
mkdir(const char *pathname, mode_t mode)
{
    intercept("mkdir", 2);
    set_errno();
    return -1;
}

int
rmdir(const char *pathname)
{
    intercept("rmdir", 2);
    set_errno();
    return -1;
}

int
chmod(const char *pathname, mode_t mode)
{
    intercept("chmod", 2);
    set_errno();
    return -1;
}

int
chown(const char *pathname, uid_t owner, gid_t group)
{
    intercept("chown", 2);
    set_errno();
    return -1;
}

int
fchmod(int fd, mode_t mode)
{
    intercept("fchmod", 2);
    set_errno();
    return -1;
}

int
fchown(int fd, uid_t uid, gid_t gid)
{
    intercept("fchown", 2);
    set_errno();
    return -1;
}

int
fsync(int fd)
{
    intercept("fsync", 2);
    set_errno();
    return -1;
}

int
ftruncate(int fd, off_t length)
{
    intercept("ftruncate", 1);
    set_errno();
    return -1;
}

int
ftruncate64(int fd, off_t length)
{
    intercept("ftruncate64", 1);
    set_errno();
    return -1;
}

int
link(const char *oldpath, const char *newname)
{
    intercept("link", 2);
    set_errno();
    return -1;
}

int
rename(const char *oldpath, const char *newpath)
{
    intercept("rename", 2);
    set_errno();
    return -1;
}

int
utimes(const char *path, const struct timeval times[2])
{
    intercept("utimes", 2);
    set_errno();
    return -1;
}

int
futimes(int fd, const struct timeval times[2])
{
    intercept("futimes", 2);
    set_errno();
    return -1;
}

int
utime(const char *path, const struct utimbuf *buf)
{
    intercept("utime", 2);
    set_errno();
    return -1;
}

int
mknod(const char *path, mode_t mode, dev_t dev)
{
    intercept("mknod", 2);
    set_errno();
    return -1;
}

int
__xmknod(int ver, const char *path, mode_t mode, dev_t *dev)
{
    intercept("__xmknod", 2);
    set_errno();
    return -1;
}

int
mkfifo(const char *path, mode_t mode)
{
    intercept("mkfifo", 2);
    set_errno();
    return -1;
}

int
unlink(const char *path)
{
    intercept("unlink", 2);
    set_errno();
    return -1;
}

int
symlink(const char *oldpath, const char *newpath)
{
    intercept("symlink", 2);
    set_errno();
    return -1;
}

int
readlink(const char *path, char *buf, size_t bufsize)
{
    intercept("readlink", 1);
    set_errno();
    return -1;
}

char *
realpath(const char *path, char *resolved)
{
    intercept("realpath", 1);
    set_errno();
    return NULL;
}

DIR *
opendir(const char *path)
{
    intercept("opendir", 2);
    set_errno();
    return NULL;
}

struct dirent *
readdir(DIR *dir)
{
    intercept("readdir\t", 2);
    set_errno();
    return NULL;
}

struct dirent *
readdir64(DIR *dir)
{
    intercept("readdir64", 2);
    set_errno();
    return NULL;
}

int
readdir_r(DIR *dir, struct dirent *entry, struct dirent **result)
{
    intercept("readdir_r", 1);
    set_errno();
    return -1;
}

int
readdir64_r(DIR *dir, struct dirent *entry, struct dirent **result)
{
    intercept("readdir64_r", 1);
    set_errno();
    return -1;
}

int
closedir(DIR *dh)
{
    intercept("closedir", 1);
    set_errno();
    return -1;
}

int
stat(const char *path, struct stat *buf)
{
    intercept("stat", 2);
    set_errno();
    return -1;
}

int
stat64(const char *path, struct stat *buf)
{
    intercept("stat64", 2);
    set_errno();
    return -1;
}

int
__fxstat(int ver, int fd, struct stat *buf)
{
    intercept("__fxstat\t", 2);
    set_errno();
    return -1;
}

int
__fxstat64(int ver, int fd, struct stat *buf)
{
    intercept("__fxstat64", 2);
    set_errno();
    return -1;
}


int
fstat(int fd, struct stat *buf)
{
    intercept("fstat", 2);
    set_errno();
    return -1;
}

int
fstat64(int fd, struct stat *buf)
{
    intercept("fstat64", 2);
    set_errno();
    return -1;
}


int
__lxstat64(int ver, const char *path, struct stat *buf)
{
    intercept("__lxstat64", 2);
    set_errno();
    return -1;
}

int
lstat(const char *path, struct stat *buf)
{
    intercept("lstat", 2);
    set_errno();
    return -1;
}

int
lstat64(const char *path, struct stat *buf)
{
    //printf("USER INVOKED LSTAT64 ON: %s !!!!! ", path);
    intercept("lstat64", 2);
    set_errno();
    return -1;
}

int
statfs(const char *path, struct statfs *buf)
{
    intercept("statfs", 2);
    set_errno();
    return -1;
}

int
statfs64(const char *path, struct statfs *buf)
{
    intercept("statfs64", 2);
    set_errno();
    return -1;
}

int
statvfs(const char *path, struct statvfs *buf)
{
    intercept("statvfs\t", 2);
    set_errno();
    return -1;
}

int
statvfs64(const char *path, struct statvfs *buf)
{
    intercept("statvfs64", 2);
    set_errno();
    return -1;
}

ssize_t
getxattr(const char *path, const char *name, void *value, size_t size)
{
    intercept("getxattr", 1);
    set_errno();
    return -1;
}

ssize_t
lgetxattr(const char *path, const char *name, void *value, size_t size)
{
    intercept("lgetxattr", 1);
    set_errno();
    return -1;
}

int
remove(const char *path)
{
    intercept("remove", 2);
    set_errno();
    return -1;
}

int
lchown(const char *path, uid_t owner, gid_t group)
{
    intercept("lchown", 2);
    set_errno();
    return -1;
}

void
rewinddir(DIR *dirp)
{
    intercept("rewinddir", 1);
    set_errno();
    return;
}

void
seekdir(DIR *dirp, off_t offset)
{
    intercept("seekdir", 2);
    set_errno();
    return;
}

off_t
telldir(DIR *dirp)
{
    intercept("telldir", 2);
    set_errno();
    return -1;
}

ssize_t
sendfile(int out_fd, int in_fd, off_t *offset, size_t count)
{
    intercept("sendfile\t", 1);
    set_errno();
    return -1;
}

ssize_t
sendfile64(int out_fd, int in_fd, off_t *offset, size_t count)
{
    intercept("sendfile64", 1);
    set_errno();
    return -1;
}

int
fcntl(int fd, int cmd, ...)
{
    intercept("fcntl", 2);
    set_errno();
    return -1;
}*/
