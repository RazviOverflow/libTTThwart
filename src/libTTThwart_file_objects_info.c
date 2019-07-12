#define _GNU_SOURCE 
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "libTTThwart_file_objects_info.h"
#include "libTTThwart_wrappers.h"
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
void upsert_inode_in_array(file_objects_info *array, const char *path, ino_t inode, char *tmp_dir){

		printf("I REACH %s\n", __func__);
    // If array has not been yet initialized, initialize it. 
	if(array->size == 0){
		initialize_array(array, 2);
	} 

	printf("I REACH %s AFTE RARRAY INITIALIZATION\n", __func__);

	// If element is already in array simply update its inode in case the
	// the new inode is different from the one already existing
	int index = find_index_in_array(array, path);

	printf("I REACH %s ATER INDEX FINDING\n", __func__);

	// A copy of tmp_dir is created and used since we don't want the original
	// tmp folder to be overwritten or concatenated with whatever else. 
	char *tmp_dir_folder_aux = strdup(tmp_dir);

	char *full_temporal_file_name ;

	uint random_name_length = 10;

	char random_name[random_name_length];

	char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.-#?!@[]()"; // could be const

	// -1 because there must be room for the trailing null byte
    for (size_t n = 0; n < random_name_length-1; n++) {
        int key = rand() % (int) (sizeof charset - 1);
        random_name[n] = charset[key];
    }
    random_name[random_name_length] = '\0';

    	printf("I CALCULATED THE RANDOM NUMBER %s\n", __func__);

	if(index >= 0){

		printf("I REACH WHERE INDEX >= 0 BECAUSE PATH IS ALREADY IN ARRAY %s\n", __func__);

		if(inode != array->list[index].inode){
			array->list[index].inode = inode;
			zlogf_time(ZLOG_DEBUG_LOG_MSG, "Updated inode (now %lu) of path %s\n", inode, path);
		}

		printf("I REACH WHERE INDEX >= 0 BECAUSE PATH IS ALREADY IN ARRAY %s || AFTER THE INDE != COMPARISON \n", __func__);

		if(!array->list[index].tmp_path){

			printf("I REACH WHERE INDEX >= 0 BECAUSE PATH IS ALREADY IN ARRAY %s || TEMPORAL PATH FROM ARRAY ENTRY IS EMPTY\n", __func__);

			// Safe concatenation of strings
			if(asprintf(&full_temporal_file_name, "%s%s", tmp_dir_folder_aux, random_name) == -1){
				fprintf(stderr, "[!] ERROR saving memory for temporal file name construction (concatenation).\n[!] ERROR: %s\n", strerror(errno));
				exit(EXIT_FAILURE);
			}

			printf("I REACH WHERE INDEX >= 0 BECAUSE PATH IS ALREADY IN ARRAY %s ||  AFTER THE ASPRINTF \n", __func__);

			if(link_wrapper(path, full_temporal_file_name) == -1){
				zlogf_time(ZLOG_DEBUG_LOG_MSG, "[!] ERROR trying to create temporal file.\n[!] ERROR: %s\n", strerror(errno));
				array->list[array->used].tmp_path = NULL;
			} else {
				array->list[array->used].tmp_path = strdup(full_temporal_file_name);
			}

			free(full_temporal_file_name);

		}
		
		printf("I REACH WHERE INDEX >= 0 BECAUSE PATH IS ALREADY IN ARRAY %s || AFTER THE TEMPORAL PATH FROM ARRAY ENTRY IS EMPTY\n", __func__);

	} else  {

		printf("I REACH WHERE PATH IS NOT IN THE ARRAY %s\n", __func__);

	    // If number of elements (used) in the array equals its size, it means
	    // the array requires more room. It's size gets doubled
		if(array->used == array->size){

			printf("I REACH WHERE PATH IS NOT IN THE ARRAY %s || RIHT BEFORE REALLOC AND ARRAY SIZE IS= %d\n", __func__, array->size);
			////printf("Size of array %X is about to get doubled.\n", &(*array));
			//printf("Size of array is about to get doubled\n");
			array->size *= 2;
			file_object_info *aux = (file_object_info *)realloc(array->list,
				array->size * sizeof(file_object_info));

			printf("I REACH WHERE PATH IS NOT IN THE ARRAY %s || AFTER REALLOC \n", __func__);
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

		printf("I REACH WHERE PATH IS NOT IN THE ARRAY|| AFTER THE CHECKING OF ARRAY USED - SIZE %s\n", __func__);

		// Creating temporal symlink. When inode == NONEXISTING_FILE_INODE
		// TMP_DIR_FOLDER_AUX is NULL.
		if(inode != NONEXISTING_FILE_INODE){
			printf("I REACH WHERE PATH IS NOT IN THE ARRAY || INODE DIFFERENT THAN NONEXISTING_FILE_INODE %s\n", __func__);
			// Safe concatenation of strings

			if(asprintf(&full_temporal_file_name, "%s%s", tmp_dir_folder_aux, random_name) == -1){
				printf("I REACH WHERE PATH IS NOT IN THE ARRAY || INSIDE ASPRINTF %s\n", __func__);
				fprintf(stderr, "[!] ERROR saving memory for temporal file name construction (concatenation).\n[!] ERROR: %s\n", strerror(errno));
				exit(EXIT_FAILURE);
			}

			if(link_wrapper(path, full_temporal_file_name) == -1){
				zlogf_time(ZLOG_DEBUG_LOG_MSG, "[!] ERROR trying to create temporal file. ERROR: %s\n", strerror(errno));
				array->list[array->used].tmp_path = NULL;
			} else {
				array->list[array->used].tmp_path = strdup(full_temporal_file_name);
			}

			free(full_temporal_file_name);

		} else {
			printf("I REACH WHERE PATH IS NOT IN THE ARRAY || ELSE %s\n", __func__);
			array->list[array->used].tmp_path = NULL;
		}

		array->list[array->used].path = strdup(path);
		array->list[array->used].inode = inode;
		array->used++;
	}

	printf("I REACH AFTER ALL THE COMPARISONS IN %s || RIGHT BEFORE FREEING\n", __func__);

	free(tmp_dir_folder_aux);
	printf("I REACH WHERE INDEX >= 0 BECAUSE PATH IS ALREADY IN ARRAY %s\n || AFTER THE FIRST FREE", __func__);

	printf("I GET OUT OF %s\n", __func__);

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

	int number_elements = array->used;
	if(index < number_elements){

		for(int i = index; i < number_elements; i++){
			array->list[i] = array->list[i+1];
		}

	} 

	array->used--;
}