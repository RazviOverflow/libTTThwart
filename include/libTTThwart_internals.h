#ifndef LIBTTHWART_INTERNALS_H_
#define LIBTTHWART_INTERNALS_H_

#include <stdbool.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdarg.h>
#include <errno.h>
#include <fcntl.h>

#define GET_PROGRAM_NAME() program_invocation_short_name

void get_fs_and_initialize_checking_functions(const char *);

void check_dlsym_error();

void print_function_and_path(const char *, const char *, const char *);

void check_parameters_properties(const char *, const char *);

int get_number_of_variable_arguments_char_pointer_type(va_list );

ino_t get_inode(const char *);

const char * sanitize_and_get_absolute_path(const char *);

const char * sanitize_and_get_absolute_path_from_dir_file_descriptor(const char *, int);

bool path_is_absolute(const char *);

int file_does_exist(const char *);

char * get_directory_from_fd(int);

int remove_directory_and_content(char *);

ino_t get_inode(const char *);

struct stat get_file_metadata(const char *);

int file_does_exist(const char *);

bool starts_with(const char *, const char *);

#endif