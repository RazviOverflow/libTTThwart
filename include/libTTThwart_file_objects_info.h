/*
	About: License

		Whatever

	Authors:

		<Razvan Raducu: https://twitter.com/Razvieu>

		<Ricardo J. Rodríguez: https://twitter.com/RicardoJRdez>
*/

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

/*
	Constant: Nonexisting file inode

	NONEXISTING_FILE_INODE - Consant whose value is 0. It's used when <get_inode>
	is invoked over a file that does not exist. In that case, <get_inode> returns
	NON_EXISTING_FILE_INODE.
*/
#define NONEXISTING_FILE_INODE 0

/*
	Typedef: file_object_info

	Represents specific metadata of each file used by any of the hooked functions.  

	Members:
		- Inode. Inode of the given file. 
		- Device id. Id of the device where the file is located.
		- File mode. File mode of the given file. 
		- Path. Absolute path of the given file. 
		- Temporal path. (OPTIONAL) Temporal hard link of the file. This structure 
		member is only set if the filesystem is EXT2, EXT3 or EXT4.
*/
typedef struct{
	ino_t inode;
	dev_t device_id;
	mode_t file_mode;
	char *path;
	char *tmp_path;
} file_object_info;

/*
	Typedef: file_objects_info

	Main structure of the whole library. It is a global array containing different
	<file_object_info> entries. 

	Members:
		- List. A <file_object_info> pointer. This pointer is used to access each
		and every <file_object_info> entry.
		- Used. Parameter that indicates how many elements the array has. 
		- Size. Parameter that indicates how big the array is.  
*/
typedef struct{
	file_object_info *list;
	size_t used;
	size_t size;
} file_objects_info;


/*
	Pointer to function: upsert_file_data_in_array

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
	Function: upsert_file_data_in_array_ext3ext4

	Implementation of upsert_file_data_in_array for ext3 and ext4 file systems. 
	The main difference is that these filesystems reuse inodes so hardlinks must
	be created in order to ensure inode consistency.

    Inserts into the given array the given path and inode. Before inserting elements
    into the given array, the array must be initialized. If there is not enough 
    room in the array to insert a new <file_object_info> element, the size of 
    the array gets doubled. After the element is inserted, "used" member of the 
    given array is postincremented. If the path already exists in the array, the 
    function updates the corresponding inode instead of inserting new element.

    Parameters: 
    	array - <file_objects_info> array to insert into.
    	path - Sanitized path to insert.
    	inode - Inode to insert.
    	tmp_dir - Temporal hardlinks directory to insert, in case there's any. 
*/
void upsert_file_data_in_array_ext3ext4(file_objects_info *array, const char *path, ino_t inode, char *tmp_dir);

/*
	Function: upsert_file_data_in_array_otherfs

	Implementation of upsert_file_data_in_array for other FS than ext3 and ext4.
	As far as I am concerned as of right now, there is no need to ensure inode
	consistency because only ext3 and ext4 reuse them. Last 
	
	Parameters:
    	array - <file_objects_info> array to insert into.
    	path - Sanitized path to insert.
    	inode - Inode to insert.
    	tmp_dir - Temporal hardlinks directory to insert, in case there's any. 

*/
void upsert_file_data_in_array_otherfs(file_objects_info *array, const char *path, ino_t inode, char *tmp_dir);

/*
	Function: initialize_array

    Initializes the given <file_objects_info> array with the given size, allocating
    the corresponding memory. The allocation is made via <calloc> and the function
    performs error checking. 

    Parameters:
    	array - Pointer to <file_objects_info> array to initialize.
    	size - Initial size to allocate memory. 
*/
void initialize_array(file_objects_info *array, size_t size);

/*
	Function: upsert_nonexisting_file_metadata_in_array

	Inserts into the given array the <file_object_info> entry with the given path
	and inode as well as other metadata just like <upsert_file_data_in_array_otherfs>
	does. The difference is that the inode will be 0 (zero), the temporal path will
	be null while device_id and file_mode will get ther default (int) value.

	Parameters:
		array - <file_objects_info> array to insert into.
    	path - Sanitized path to insert.
    	inode - Inode to insert.
*/
void upsert_nonexisting_file_metadata_in_array(file_objects_info *array, const char *path, ino_t inode);

/*	
	Function: free_array

    Frees the memory used by the given array. This function is ment to be called 
    at the end of the program, in <after_main> destructor function. 

    Parameters:
		array - Array of type <file_objects_info> that must be freed. 
*/
void free_array(file_objects_info *array);

/*
	Function: find_index_in_array

    Find the index of the given path in the given array. The first check performed
    is asserting array size is bigger than 0 (zero) because othwerwise means the
    array has not yet been initialized, so there is no way the element could be 
    found in it. If array size is bigger than 0 (zero) the search is performed in
    linear, sequential manner. From the first element to the last.

    Parameters:
		array - Array of type <file_objects_info> to go across and search in. 
		Path - Path to search. 

    Returns:
    	Index of the element (if found); otherwise -1.  
*/
int find_index_in_array(file_objects_info *array, const char *path);


/*
	Function: get_from_array_at_index

   	Retrieves the <file_object_info> element at the given index in the given array. 

	Parameters:
		array - Array of type <file_objects_info> to go across and search in. 
		index - Index (postion) from "array" to retrieve. 

   	Returns:
   		<file_object_info> element at the given index. 
*/
file_object_info get_from_array_at_index(file_objects_info *array, int index);

/*
	Function: remove_from_array_at_index

	Removes the element at index "index" from the array. Please note it's index,
	not position. Indexes start at 0.

	Parameters:
		array - Array of type <file_objects_info> to go across. 
		index - Index (postion) from "array" to delete. 

	Returns:

*/
void remove_from_array_at_index(file_objects_info *array, int index);

#endif