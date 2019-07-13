#define _GNU_SOURCE 
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "libTTThwart_file_objects_info.h"
#include "libTTThwart_global_variables.h"
#include "libTTThwart_wrappers.h"
#include "libTTThwart_internals.h"
#include "zlog.h"

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
void upsert_file_data_in_array_ext3ext4(file_objects_info *array, const char *path, ino_t inode, char *tmp_dir){

	printf("IM USING UPSERT FOR EXT3EXT4\n");

	struct stat local_stat = get_file_metadata(path);

	if(!starts_with("/etc", path)){
		ino_t local_inode = local_stat.st_ino;

		if(inode != local_inode){
			zlogf_time(ZLOG_INFO_LOG_MSG, "[+][!] WARNING! TOCTTOU DETECTED! [+][!]\n Inode of <%s> has changed since it was previously invoked. Threat detected when trying to upsert data. Inode was <%lu> and now it is <%lu>. \n [#] PROGRAM %s ABORTED [#]\n\n", path, inode, local_inode, GET_PROGRAM_NAME());
			fprintf(stderr,"[+][!] WARNING! TOCTTOU DETECTED!. [!][+]\n[#] PROGRAM %s ABORTED [#]\n[#] Check logs for more info [#]\n[!] LOGIFLE: %s [!]\n", GET_PROGRAM_NAME(), zlog_get_log_file_name());
			fflush(stdout);
			exit(EXIT_FAILURE);
		}
	}


    // If array has not been yet initialized, initialize it. 
	if(array->size == 0){
		initialize_array(array, 2);
	} 

	// If element is already in array simply update its inode in case the
	// the new inode is different from the one already existing
	int index = find_index_in_array(array, path);

	uint random_name_length = 25;
	uint full_random_name_length;

	char random_name[random_name_length];

	char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.-#?!@[]()"; // could be const

	// -1 because there must be room for the trailing null byte
    for (size_t n = 0; n < random_name_length-1; n++) {
        int key = rand() % (int) (sizeof charset - 1);
        random_name[n] = charset[key];
    }
    random_name[random_name_length] = '\0';
   
    // +1 because of trailing null byte
    full_random_name_length = strlen(tmp_dir) + strlen("/") + strlen(random_name) + 1;

    char full_random_file_name[full_random_name_length];

    snprintf(full_random_file_name, full_random_name_length, "%s/%s", tmp_dir, random_name);

	if(index >= 0){

		if(inode != array->list[index].inode){
			array->list[index].inode = inode;
			zlogf_time(ZLOG_DEBUG_LOG_MSG, "Updated inode (now %lu) of path %s\n", inode, path);
		}

		if(!array->list[index].tmp_path){

			if(link_wrapper(path, full_random_file_name) == -1){
				zlogf_time(ZLOG_DEBUG_LOG_MSG, "[!] ERROR trying to create temporal file.\n[!] ERROR: %s\n", strerror(errno));
				array->list[array->used].tmp_path = NULL;
			} else {
				array->list[array->used].tmp_path = strdup(full_random_file_name);
			}

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
				fprintf(stderr, "[!] ERROR  trying to realloc size for array in upsert inode process.\n[!] ERROR: %s\n", strerror(errno));
				exit(EXIT_FAILURE);
			} else {
				array->list = aux;
			}

        	//Initializing new elements of realocated array-
			memset(&array->list[array->used], 0, sizeof(file_object_info) * (array->size - array->used));
		}

		// Creating temporal symlink. When inode == NONEXISTING_FILE_INODE
		// TMP_DIR_FOLDER_AUX is NULL.
		if(inode != NONEXISTING_FILE_INODE){

			if(link_wrapper(path, full_random_file_name) == -1){
				zlogf_time(ZLOG_DEBUG_LOG_MSG, "[!] ERROR trying to create temporal file. ERROR: %s\n", strerror(errno));
				array->list[array->used].tmp_path = NULL;
			} else {
				array->list[array->used].tmp_path = strdup(full_random_file_name);
			}


		} else {
			array->list[array->used].tmp_path = NULL;
		}

		array->list[array->used].device_id = local_stat.st_dev;
		array->list[array->used].file_mode = local_stat.st_mode;
		array->list[array->used].path = strdup(path);
		array->list[array->used].inode = inode;
		array->used++;
	}

}

void upsert_file_data_in_array_otherfs(file_objects_info *array, const char *path, ino_t inode, char *tmp_dir __attribute__((unused))){

	printf("IM USING UPSERT FOR OTHER FS\n");

	struct stat local_stat = get_file_metadata(path);

	if(!starts_with("/etc", path)){
		ino_t local_inode = local_stat.st_ino;

		if(inode != local_inode){
			zlogf_time(ZLOG_INFO_LOG_MSG, "[+][!] WARNING! TOCTTOU DETECTED! [+][!]\n Inode of <%s> has changed since it was previously invoked. Threat detected when trying to upsert data. Inode was <%lu> and now it is <%lu>. \n [#] PROGRAM %s ABORTED [#]\n\n", path, inode, local_inode, GET_PROGRAM_NAME());
			fprintf(stderr,"[+][!] WARNING! TOCTTOU DETECTED!. [!][+]\n[#] PROGRAM %s ABORTED [#]\n[#] Check logs for more info [#]\n[!] LOGIFLE: %s [!]\n", GET_PROGRAM_NAME(), zlog_get_log_file_name());
			fflush(stdout);
			exit(EXIT_FAILURE);
		}
	}

	if(array->size == 0){
		initialize_array(array, 2);
	} 

	int index = find_index_in_array(array, path);

	if(index >= 0){
		if(inode != array->list[index].inode){
			array->list[index].inode = inode;
			zlogf_time(ZLOG_DEBUG_LOG_MSG, "Updated inode (now %lu) of path %s\n", inode, path);
		}
	} else  {

		if(array->used == array->size){

			array->size *= 2;
			file_object_info *aux = (file_object_info *)realloc(array->list,
				array->size * sizeof(file_object_info));

			if(!aux){
				fprintf(stderr, "Error trying to realloc size for array in upsert inode process.\n");
				exit(EXIT_FAILURE);
			} else {
				array->list = aux;
			}

			memset(&array->list[array->used], 0, sizeof(file_object_info) * (array->size - array->used));

		}
		array->list[array->used].device_id = local_stat.st_dev;
		array->list[array->used].file_mode = local_stat.st_mode;
		array->list[array->used].path = strdup(path);
		array->list[array->used].inode = inode;
		array->used++;
	}

}


void upsert_nonexisting_file_metadata_in_array(file_objects_info *array, const char *path, ino_t inode){
    
    // If array has not been yet initialized, initialize it. 
	if(array->size == 0){
		initialize_array(array, 2);
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
		array->list[array->used].tmp_path = NULL;
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
		free(array->list[i].tmp_path);
		array->list[i].tmp_path = NULL;
	}

	free(array->list);
	array->list = NULL;

	array->used = 0;
	array->size = 0;

	free(g_temp_dir);

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

	file_object_info aux = get_from_array_at_index(array, index);

	char *temporal_file_to_delete = aux.tmp_path;

	zlogf_time(ZLOG_DEBUG_LOG_MSG, "[+] Temporal file: %s is about to be deleted.\n", temporal_file_to_delete);
	zlog_flush_buffer();

	if(remove(temporal_file_to_delete) == -1){
		// The execution ought to never enter this block.
		fprintf(stderr, "[!] ERROR trying to delete temporal link previously created with name: %s.\n[!] ERROR: %s\n", aux.tmp_path, strerror(errno));
		zlogf_time(ZLOG_INFO_LOG_MSG, "[!] ERROR trying to delete temporal link previously created with name: %s.\n[!] ERROR: %s\n", aux.tmp_path, strerror(errno));
		zlog_flush_buffer();
	}

	int number_elements = array->used;
	if(index < number_elements){

		for(int i = index; i < number_elements; i++){
			array->list[i] = array->list[i+1];
		}

	} 

	array->used--;
	zlogf_time(ZLOG_DEBUG_LOG_MSG, "[+] Temporal file: %s was deleted.\n", temporal_file_to_delete);
	zlog_flush_buffer();
}