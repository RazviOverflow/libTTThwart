#ifndef FILEOBJECTINFOS
#define FILEOBJECTINFOS


#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

typedef struct{
  char *path;
  int inode;
} FileObjectInfo;

typedef struct{
	FileObjectInfo * list;
	size_t used;
	size_t size;
} FileObjectsInfo;

FileObjectsInfo* Initialize(size_t);
void Insert(FileObjectsInfo*, const char *, ino_t);
void Free(FileObjectsInfo *);
int FindIndex(FileObjectsInfo *, const char *);
FileObjectInfo Get(FileObjectsInfo *, int);
//void CreateFileObjectInfo(char*, ino_t); //private

#endif //FILEOBJECTINFOS