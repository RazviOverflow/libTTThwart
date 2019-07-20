/*
	About: License

		Whatever

	Authors:

		<Razvan Raducu: https://twitter.com/Razvieu>

		<Ricardo J. Rodríguez: https://twitter.com/RicardoJRdez>
		
	Code: 
		You can find the source code associated with this header <here: https://github.com/RazviOverflow/ld.so.preload/blob/master/src/libTTThwart_internals.c>.	
*/

#ifndef LIBTTHWART_INTERNALS_H_
#define LIBTTHWART_INTERNALS_H_

#include <stdbool.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdarg.h>
#include <errno.h>
#include <fcntl.h>

/*
	Constant: GET_PROGRAM_NAME()
	
	Function used to retrieve the actual process name. 
*/
#define GET_PROGRAM_NAME() program_invocation_short_name


/*
	Function: check_parameters_properties

    Checks properties of the given parameters, this is, the given path and 
    inode. Checking properties in this context means checking if a 
    <file_object_info> with the same path already exists in the array. If it
    doesn't, insert it; othwerwise (if it does) compare 
    the given inode and the inode of the <file_object_info> as well as the
    file mode and the device id. If they're equal continue execution; otherwise
    abort execution because a possible TOCTTOU is detected.

    Checking parameters is different based on the FILE SYSTEM of the given path.
    At the moment, the function only considers two file systems:

    	- EXT2/EXT3/EXT4 (their magic number is the same). When the path is located
    	in a ext2/3/4 file system, temporal hardlinks are created in order to
    	ensure the inode is not reused. One "feature" of such filesystems is that 
    	they reuse inode as soon as a given inode has 0 links pointing to it. 
    	- OTHER FS. When the path is allocated on any other possible file system
    	the checkings performed are the same with the exception of temporal hard
    	links creation. We do not need hard links because inodes are not reused. 

    In order to see this function initialization, please refer to <get_fs_and_initialize_checking_functions>.

    Parameters:
    	path - absolute path of the file whose metadata must be checked.
    	caller_function_name -  name of the function from which the checking was
    		called. 
*/
void check_parameters_properties(const char *path, const char *caller_function_name);

/*	
	Function: sanitize_and_get_absolute_path

	Function to get full path of a given parameter without resolving, expanding
	symbolic links. That's why realpath() is useless. 
	Based on: https://stackoverflow.com/questions/4774116/realpath-without-resolving-symlinks/34202207#34202207

	Parameters:
		src - Path to sanitize and absolutize.

	Returns:
		The sanitized and absolutized path. 

*/
const char * sanitize_and_get_absolute_path(const char *);

/*
	Function: sanitize_and_get_absolute_path_from_dir_file_descriptor

	Function to get full path of a given parameter without resolving, expanding
	symbolic links but using a directory file descriptor as current working dir. 
	It is assumed that the file is indeed within that directory. The function 
	translates the file descriptor into the actual directory (string).

	Parameters: 
		src - Path to sanitize according to directory_fd.
		directory_fd - File descriptor pointing to certain directroy. 

	Returns:
		The sanitized function according to directory_fd.
*/
const char * sanitize_and_get_absolute_path_from_dir_file_descriptor(const char *src, int directory_fd);

/*
	Function: get_directory_from_fd

	Function used to retrieve as string the full directory path pointed to
	by a given file descriptor. 
	
	Parameters:
		directory_fd - File descriptor pointing to some directroy.

	Returns:
		The name of the directory pointed to by directory_fd. 

*/
char * get_directory_from_fd(int directory_fd);

/*
	Function: get_fs_and_initialize_checking_functions

	Determines what the file system of a given path is and, based
	on that, initialize <check_parameters_properties> function accordingly. Please
	refer to that function in order to know what different initializations means. 

	Parameters:
		path - Absolute or relative path whose filesystem must be retrieved. 
*/
void get_fs_and_initialize_checking_functions(const char *path);

/*
	Function: check_dlsym_error
	
	Just a wrapper to check dlsym errors. This wrapper complies wit the recommendations
	given at the <offical documentation: http://man7.org/linux/man-pages/man3/dlsym.3.html>. 
*/
void check_dlsym_error();

/*
	Function: print_function_and_path

	Function used to log what function was called, what path it was called with
	and its sanitized version. 

	Parameters:
		func - Function that was originally called.
		path - What path is was passwed into as parameter.
		sanitized_path - Sanitized path of <path> argument. 
*/
void print_function_and_path(const char *func, const char *path, const char *sanitized_path);

/*
	Function: get_number_of_variable_arguments_char_pointer_type

	Function used to calculate the number of arguments given a variable char argument
	list.

	Parameters:
		variable_arguments - List of variable arguments to go across.

	Returns:
		The number of char parameters. 
*/
int get_number_of_variable_arguments_char_pointer_type(va_list variable_arguments);

/*
	Function: get_inode

	Function used to get the corresponding inode of a given path.

	Parameters:
		path - Path whose inode must be retrieved. 

	Returns:
		The inode. 
*/
ino_t get_inode(const char *path);

/*
	Function: path_is_absolute

	Function used to determine whether a path is absolute.

	Parameters:
		path - Path to check whether it's absolute.

	Returns:
		True if path is asbolute; otherwise False. 
*/
bool path_is_absolute(const char *path);

/*
	Function: file_does_exist

	Function used to determine whether a file exists at the moment of invocation. 
	In order to do so, it calls <open> and check the value of the returned 
	file descriptior.
	
	Parameters:
		pathname - Path of the file to check. 

	Returns:
		0 for false.
		1 for true. 
*/
int file_does_exist(const char *pathname);

/*
	Function: remove_directory_and_content

	Funcrtion used to recursively delete a given directory.

	Parameters:
		path_to_remove - Path to recorsively remove. 

	Returns:
		Value indicating whethere there were errors.
*/
int remove_directory_and_content(char *path_to_remove);

/*
	Function: get_file_metadata

	Function used to retrieve metadata of a given path. In order to do so, the
	function calls <open>.

	Parameters:
		path - Path whose metadata must be retrieved. 

	Returns:
		struct stat containing all the metadata of the given path. 
*/
struct stat get_file_metadata(const char *path);

/*
	Function: starts_with

	Function used to check whether the given str parameter start with the given
	pre parameter.

	Parameters:
		pre - Preffix to check.
		str - String to check if starts with "pre".

	Returns:
		True if str starts with pre.
		False otherwise. 
*/
bool starts_with(const char *pre, const char *str);

#endif