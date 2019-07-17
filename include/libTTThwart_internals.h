/*
	About: License

		Whatever

	Authors:

		<Razvan Raducu: https://twitter.com/Razvieu>

		<Ricardo J. Rodríguez: https://twitter.com/RicardoJRdez>
		
	Compilation:
		Simply use 'make' from the root directory. If you want debug messages 
		compile with 'make debug'
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