/*
	About: License

		Copyright 2019 Razvan Raducu and Ricardo J. Rodriguez

		This file is part of libTTThwart.so.

	    libTTThwart.so is free software: you can redistribute it and/or modify
	    it under the terms of the GNU General Public License as published by
	    the Free Software Foundation, either version 3 of the License, or
	    (at your option) any later version.

	    libTTThwart.so is distributed in the hope that it will be useful,
	    but WITHOUT ANY WARRANTY; without even the implied warranty of
	    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	    GNU General Public License for more details.

	    You should have received a copy of the GNU General Public License
	    along with libTTThwart.so.  If not, see <https://www.gnu.org/licenses/>.

	Authors:

		<Razvan Raducu: https://twitter.com/Razvieu>

		<Ricardo J. Rodríguez: https://twitter.com/RicardoJRdez>
		
	Code: 
		You can find the source code associated with this header <here: https://github.com/RazviOverflow/ld.so.preload/blob/master/src/libTTThwart_wrappers.c>.		
*/

#ifndef LIBTTHWART_WRAPPERS_H
#define LIBTTHWART_WRAPPERS_H

#include <stdarg.h>
#include <stdio.h>

/*
	About: Wrappers' scope.
	
	All the wrappers defined here are used for function calls within the library.
	The use of wrappers is very important because it avoids recursive calls of
	whatever function. For example, let's say there is a need to call open within
	the library. If the libarary were to call open(), since open() is a hooked
	function, the call would be recursive. We need to directly call original
	functions from within the library and we achieve that by using wrappers.
*/

/*
	Function: dlsym_wrapper

    The correct way to test for an error is to call dlerror() to clear any old 
    error conditions, then call dlsym(), and then call dlerror() again, saving 
    its return value into a variable, and check whether this saved value is not NULL.
    Please refer to <Linux Programmer's Manual (man-pages) official dlsym() docs: http://man7.org/linux/man-pages/man3/dlsym.3.html>

    Parameters:
    	original_function - Function's name to retrieve from the dynamic linker.
*/
extern void * dlsym_wrapper(const char *original_function);

/*
	Function: open_wrapper

    The open wrapper guarantees, insures original_open is initialized.
    It's used by other inner functions in order to avoid <open(): http://man7.org/linux/man-pages/man2/open.2.html> recursivity
    and overhead. In adittion, it deals with ellipsis (variable 
    arguments) since open is a variadic function.

	Parameters:
		path - Absolute or retalive path to open.
		flags - Access modes.
		variable_arguments - Ellipsis, variable arguments (creation flags and/or file status flags).
	
	Returns:
		The new file descriptor, or -1 if an error occurred (in which case, errno 
		is set appropriately).

*/
extern int open_wrapper(const char *path, int flags, va_list variable_arguments);

/*
	Function: open64_wrapper

	Exactly the same as <open_wrapper> but for files larger than 2 GiB-1 size. 
	Please refer to <Large-file support Linux extensions: https://en.wikipedia.org/wiki/Large-file_support>

	Parameters:
		path - Absolute or retalive path to open.
		flags - Access modes.
		variable_arguments - Ellipsis, variable arguments (creation flags and/or file status flags).

	Returns:
		The new file descriptor, or -1 if an error occurred (in which case, errno 
		is set appropriately).
*/
extern int open64_wrapper(const char* path, int flags, va_list variable_arguments);

/*	
	Function: openat_wrapper

	Openat wrapper is exactly the same as <open_wrapper> but for
	<openat(): http://man7.org/linux/man-pages/man2/open.2.html>.

	Parameters:
		dirfd - File descriptor pointing to some directory. 
		path - Absolute or retalive path to open.
		flags - Access modes.
		variable_arguments - Ellipsis, variable arguments (creation flags and/or file status flags).

	Returns:
		The new file descriptor, or -1 if an error occurred (in which case, errno 
		is set appropriately).

*/
extern int openat_wrapper(int dirfd, const char *path, int flags, va_list variable_arguments);

/*

	Function: execlX_wrapper

	Wrapper for all execlX functions family. This wrapper treats the variable
	arguments and calls the corresponding execlX function according to:
	[function argument value] : [execlX function]

	0 : execl https://code.woboq.org/userspace/glibc/posix/execl.c.html

	1 : execlp https://code.woboq.org/userspace/glibc/posix/execlp.c.html

	2 : execle https://code.woboq.org/userspace/glibc/posix/execle.c.html

	Additional info: https://code.woboq.org/userspace/glibc/posix/execl.c.html

	Parameters:
		function - Number indicating the execl* family function according to the
		scheme above. 
		pathname - Path to the program to execute.
		argc - Arguments of the program to execute. 
		variable_arguments - Lost of variable arguments, ellipsis, of exec* function. 

	Returns:
		Returns the return value of the called execl* function or -1 in case of errors. 

*/
extern int execlX_wrapper(int function, const char *pathname, const char *arg, va_list variable_arguments);


/*
	Function: execv_wrapper

	Wrapper for execv() function. 

	Parameters:
		pathname - Name of program to execute. 
		argv - Vector of arguments to that program.

	Returns:
		Original return value of execv call. 

*/
extern int execv_wrapper(const char *pathname, char * const *argv);

/*
	Function: execvp_wrapper
	
	Wrapper for execvp() function.

	Parameters:
		file - Path to the file being executed. 
		argv - Vector of arguments to that file.

	Returns:
		Original return value of execvp call. 
*/
extern int execvp_wrapper(const char *file, char * const *argv);

/*
	Function: execve_wrapper

	Wrapper for execve() function. 

	Parameters:
		pathname - Path to program to execute.
		argv - Arguments to that program.
		envp - Environment paramenters. 

	Returns:
		Original return value of execve call.
*/
extern int execve_wrapper(const char *pathname, char * const *argv, char * const *envp);

/*
	Function: execvpe_wrapper

	Wrapper for execvpe() function.

	Paramteres:
		file - Path to the file being executed.
		argv - Arguments to that file.
		envp - Environment parameters. 

	Returns:
		Original return value of execvpe call. 
*/
extern int execvpe_wrapper(const char *file, char * const *argv, char * const *envp);

/*
	Function: chdir_wrapper

	Wrapper for chdir() function.

	Parameters:
		path - Path to where currend working directory should be changed. 

	Returns:
		Original return value of chdir call. 
*/
extern int chdir_wrapper(const char *path);

/*
	Function: fopen_wrapper

	Wrapper for fopen() function.

	Parameters:
		path - Path of file to open.
		mode - Opening mode. 

	Returns:
		Original return value of chdir call.
*/
extern FILE* fopen_wrapper(const char *path, const char *mode);

/*
	Function: mkdir_wrapper

	Wrapper for mkdir() function.

	Parameters:
		pathname - Path of directory to create.
		mode - Creation mode

	Returns:
		Original return value of mkdir call.
*/
extern int mkdir_wrapper(const char *pathname, mode_t mode);

/*
	Function: symlink_wrapper

	Wrapper for symlink() function.

	Parameters:
		target - File or directory to which the new simbolic link will point to.
		linkpath - Path of the new simbolic link. 

	Returns:
		Original return value of symlink call.
*/
extern int symlink_wrapper(const char *target, const char *linkpath);

/*
	Function: link_wrapper

	Wrapper for link() function. 

	Parameters:
		oldpath - File or directory to which the new link (hard) will point to.
		newpath - Path of the new link.

	Returns:
		Original return value of link call. 
*/
extern int link_wrapper(const char *oldpath, const char *newpath);

/*
	Function: remove_wrapper

	Wrapper for remove() function.

	Parameters:
		pathname - File or directory to remove. 

	Returns:
		Original return value of remove call.
*/
extern int remove_wrapper(const char *);

#endif