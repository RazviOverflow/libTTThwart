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
	ino_t inode;
	dev_t device_id;
	mode_t file_mode;
	char *path;
	char *tmp_path;
} file_object_info;

typedef struct{
	file_object_info *list;
	size_t used;
	size_t size;
} file_objects_info;


/*
	Upserts (updates or inserts) the corresponding data of a given file into the
	global structure (array) that's used to assure files consistency between 
	operations. The data that's being saved is:
		- Inode
		- Device ID
		- File Mode
		- Absolute path
		- Path o temporal link (optional, defaults to NULL)

	In addition, some operations are as well performed in order to harden race
	conditions exploitability. I.e:
		- When function gets invoked, one of the parameters it receives is the
		actual inode corresponding to the given path (the inode is obtained 
		elsewhere). The function then obtains the inode of the given path (amongst
		many other metadata such as the device id) and compares them. If they're
		not equal, the program shall be aborted. 

	It's a function pointer because depending on the actual FS its implementation
	will be different.
*/
void (*upsert_file_data_in_array)(file_objects_info *, const char *, ino_t, char *);

/*
	Implementation of upsert_file_data_in_array for ext3 and ext4 file systems. 
	The main difference is that these filesystems reuse inodes so hardlinks must
	be created in order to ensure inode consistency.
*/
void upsert_file_data_in_array_ext3ext4(file_objects_info *, const char *, ino_t, char *);

/*
	Implementation of upsert_file_data_in_array for other FS than ext3 and ext4.
	As far as I am concerned as of right now, there is no need to ensure inode
	consistency because only ext3 and ext4 reuse them. Last 
*/
void upsert_file_data_in_array_otherfs(file_objects_info *, const char *, ino_t, char *);

// -- Array operations -- //
void initialize_array(file_objects_info *, size_t);

void upsert_nonexisting_file_metadata_in_array(file_objects_info *, const char *, ino_t);

void free_array(file_objects_info *);

int find_index_in_array(file_objects_info *, const char *);

file_object_info get_from_array_at_index(file_objects_info *, int);

void remove_from_array_at_index(file_objects_info *, int);


#endif