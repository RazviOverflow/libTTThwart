#include "fileobjectinfos.h"


//TODO malloc family functions error checking

void Initialize(FileObjectInfos * array, size_t initialSize){

	array->list = (FileObjectInfo *) calloc(initialSize, sizeof(FileObjectInfo));
	
	array->used = 0;
	array->size = initialSize;

	//Elements of array are contiguous
	//memset(&array->list[array->used], 0, sizeof(FileObjectInfo) * initialSize);
	

}

/*
FileObjectInfo CreateFileObjectInfo(char * path, ino_t inode){
	FileObjectInfo fileObjectInfo;
	fileObjectInfo.path = path;
	fileObjectInfo.inode = inode;
	return fileObjectInfo;
}
*/

void Insert(FileObjectInfos * array, char * path, ino_t inode){
	if(array->used == array->size){
		array->size *= 2;
		array->list = (FileObjectInfo *)realloc(array->list,
			array->size * sizeof(FileObjectInfo));

		//Initializing new elements of realocated array
		memset(&array->list[array->used], 0, sizeof(FileObjectInfo) * (array->size - array->used));


	}

	//FileObjectInfo fileObjectInfo = CreateFileObjectInfo(path, inode);

	//array->list[array->used].path = (const char *)malloc(strlen(path)+1);
	//strcpy(array->list[array->used].path, path);

	array->list[array->used].path = strdup(path);
	array->list[array->used].inode = inode;

	array->used++;

}

void Free(FileObjectInfos * array){

	for(int i = 0; i < array->used; i++){
		free(array->list[i].path);
		array->list[i].path = NULL;
	}

	free(array->list);
	array->list = NULL;

	array->used = 0;
	array->size = 0;

}