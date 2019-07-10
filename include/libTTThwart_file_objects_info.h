#ifndef FILE_OBJECTS_INFO_H_
#define FILE_OBJECTS_INFO_H_

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>           /* Definition of AT_* constants */
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define NONEXISTING_FILE_INODE 0

typedef struct{
	char *path;
	char *tmp_path;
	ino_t inode;
} file_object_info;

typedef struct{
	file_object_info *list;
	size_t used;
	size_t size;
} file_objects_info;

// -- Array operations -- //
void initialize_array(file_objects_info *, size_t);
void upsert_inode_in_array(file_objects_info *, const char *, ino_t, char *);
void free_array(file_objects_info *);
int find_index_in_array(file_objects_info *, const char *);
file_object_info get_from_array_at_index(file_objects_info *, int);
void remove_from_array_at_index(file_objects_info *, int);
char* rand_string(size_t);

#endif