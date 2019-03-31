#ifndef FILEOBJECTINFOS
#define FILEOBJECTINFOS

typedef struct{
  const char *path;
  int inode;
} FileObjectInfo;

typedef struct{
	FileObjectInfo *array;
	size_t used;
	size_t size;
}

void Initialize(FileObjectInfos*, size_t);
bool Insert(FileObjectInfos*, FileObjectInfo, ino_t);
void Free(FileObjectInfos *);
FileObjectInfo Find(char* );
void CreateFileObjectInfo(char*, ino_t); //private

#endif FILEOBJECTINFOS