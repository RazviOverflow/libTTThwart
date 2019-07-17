#ifndef LIBTTHWART_INTERNALS_H_
#define LIBTTHWART_INTERNALS_H_

#include <stdbool.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdarg.h>
#include <errno.h>
#include <fcntl.h>

#define GET_PROGRAM_NAME() program_invocation_short_name


/*
	Function used to determine what the file system of a given path is and, based
	on that, initialize <check_parameters_properties> function accordingly. Please
	refer to that function in order to know what different initializations means. 
*/
void get_fs_and_initialize_checking_functions(const char *path);

/*
	Just a wrapper to check dlsym errors. This wrapper complies wit the recommendations
	given at the offical documentation. http://man7.org/linux/man-pages/man3/dlsym.3.html
*/
void check_dlsym_error();

/*
	Function used to log what function was called, what path it was called with
	and its sanitized version. 
*/
void print_function_and_path(const char *func, const char *path, const char *sanitized_path);

/*
	Function used to calculate the number of arguments given a variable char argument
	list.

	Returns:
		The number of char parameters. 
*/
int get_number_of_variable_arguments_char_pointer_type(va_list variable_arguments);

/*
	Function used to get the corresponding inode of a given path.

	Returns:
		The inode. 
*/
ino_t get_inode(const char *path);

/*
	Function used to determine whether a path is absolute.
*/
bool path_is_absolute(const char *path);

/*
	Function used to determine whether a file exists at the moment of invocation. 
	In order to do so, it calls <open> and check the value of the returned 
	file descriptior.

	Returns:
		0 for false.
		1 for true. 
*/
int file_does_exist(const char *pathname);

/*
	Funcrtion used to recursively delete a given directory.

	Returns:
		Value indicating whethere there were errors.
*/
int remove_directory_and_content(char *path_to_remove);

/*
	Function used to retrieve metadata of a given path. In order to do so, the
	function calls <open>.

	Returns:
		struct stat containing all the metadata of the given path. 
*/
struct stat get_file_metadata(const char *path);

/*
	Function used to check whether the given str parameter start with the given
	pre parameter.

	Returns:
		True if str starts with pre.
		False otherwise. 
*/
bool starts_with(const char *pre, const char *str);

#endif