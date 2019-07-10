#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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
   	printf("HE ENTRADO EN UPSERTINODE IN ARRAY \n");
    // If array has not been yet initialized, initialize it. 
	if(array->size == 0){
		initialize_array(array, 2);
	} 
	// If element is already in array simply update its inode in case the
	// the new inode is different from the one already existing
	int index = find_index_in_array(array, path);

	char *random_name = rand_string(1);

	if(index >= 0){
		printf("HE ENTRADO EN INDEX >= 0\n");
		if(inode != array->list[index].inode){
			array->list[index].inode = inode;
			zlogf_time(ZLOG_DEBUG_LOG_MSG, "Updated inode (now %lu) of path %s\n", inode, path);
		}

		if(!array->list[index].tmp_path){
			printf("HE ENTRADO INDEX PATH CUANDO ES NULL IN ARRAY DENTRO DE >= 0 \n");

			printf("SE HA CALCULADO EL NOMBRE RANDOM: %s\n", random_name);

			printf("SRECIBIDO %s\n", tmp_dir);

			char *tmp_file_path = strcat(tmp_dir, random_name);

			printf("SE VA A INTENTAR CREAR EL FICHERO TEMPORAL\n");
			printf("SE VA A INTENTAR %s\n", tmp_dir);

			if(symlink_wrapper(path, random_name) == -1){
				fprintf(stderr, "[!] ERROR trying to create temporal file.\n");
				array->list[array->used].tmp_path = NULL;
			} else {
				array->list[array->used].tmp_path = strdup(tmp_file_path);
			}
		}
		

	} else  {
		printf("HE ENTRADO INDEX PATH NO ESTA EN LA ESTRUCTURA\n");
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
				fprintf(stderr, "[!] ERROR  trying to realloc size for array in upsert inode process.\n");
				exit(EXIT_FAILURE);
			} else {
				array->list = aux;
			}

        //Initializing new elements of realocated array
			memset(&array->list[array->used], 0, sizeof(file_object_info) * (array->size - array->used));
		}

		// Creating temporal symlink

		if(inode != NONEXISTING_FILE_INODE){

			printf("SRECIBIDO %s\n", tmp_dir);

			char *tmp_file_path = strcat(tmp_dir, random_name);

			printf("SE VA A INTENTAR CREAR EL FICHERO TEMPORAL\n");
			printf("SE VA A INTENTAR %s\n", tmp_dir);

			if(symlink_wrapper(path, random_name) == -1){
				fprintf(stderr, "[!] ERROR trying to create temporal file.\n");
				array->list[array->used].tmp_path = NULL;
			} else {
				array->list[array->used].tmp_path = strdup(tmp_file_path);
			}
		} else {
			array->list[array->used].tmp_path = NULL;
		}

		array->list[array->used].path = strdup(path);
		array->list[array->used].inode = inode;
		array->used++;
	}
		printf("HE SALIDO DE UPSERTINODE IN ARRAY \n");

}

// 
char *rand_string(size_t length)
{
    static char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789,.-#'?!"; // could be const
	char *randomString = NULL;

	if (length) {
	    randomString = malloc(length +1); // sizeof(char) == 1, cf. C99

	    if (randomString) {
	        int l = (int) (sizeof(charset) -1); // (static/global, could be const or #define SZ, would be even better)
	        int key;  // one-time instantiation (static/global would be even better)
	        for (uint n = 0;n < length;n++) {        
	            key = rand() % l;   // no instantiation, just assignment, no overhead from sizeof
	            randomString[n] = charset[key];
	        }

	        randomString[length] = '\0';
	    }
	}

	return randomString;
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