/* RazviOverflow
 This file must be compiled with the following command:
	gcc -shared -fPIC libTTThwart.c -o libTTThwart.so -ldl
 You can then execute the vulnerable code with:
 	LD_PRELOAD=$PWD/libTTThwart.so ./vulnerable tryout

Iint lstat64 (const char *__restrict __file
*/
#define _GNU_SOURCE

#include <dlfcn.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdbool.h>

#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>


// https://github.com/torvalds/linux/blob/master/include/uapi/asm-generic/fcntl.h
//#define O_RDONLY  00000000

static int (*old_xstat)(int ver, const char *path, struct stat *buf) = NULL;
static int (*old_lxstat)(int ver, const char *path, struct stat *buf) = NULL;
static int (*old_xstat64)(int ver, const char *path, struct stat64 *buf) = NULL;
static int (*old_open)(const char *path, int flags) = NULL; 

//TODO malloc family functions error checking
//TODO find
//TODO implement logger and replace printf family with corresponding log level


//#################### fileobjectsinfo.c #######################
typedef struct{
  char *path;
  int inode;
} FileObjectInfo;

typedef struct{
    FileObjectInfo * list;
    size_t used;
    size_t size;
} FileObjectsInfo;

//########## GLOBAL VARIABLES ########################
FileObjectsInfo g_array;
//####################################################

void check_and_initialize_array(){
    if(g_array.size == 0){
        Initialize(&g_array, 2);
    }
}

void check_dlsym_error(){
  char * error = dlerror();
  if(error != NULL){
    printf("There were errors while retrieving the original function from the dynamic linker/loader.\nDlsym error: %s\n", error);
    exit(EXIT_FAILURE);
  }
}


/*
    Initializes the given array with the given size, allocating
    the corresponding memory.
*/
void Initialize(FileObjectsInfo *array, size_t size){
    printf("we reach here! 1\n");
    fflush(stdout);
    

    array->list = (FileObjectInfo *) calloc(size, sizeof(FileObjectInfo));
    printf("we reach here! 2\n");
    fflush(stdout);
    array->used = 0;
    array->size = size;
    printf("we reach here! 3\n");
    fflush(stdout);
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

/*
    Inserts into the given array the given path and inode.  
    Before inserting elements into the given array, the array 
    must be initialized.
    If there is not enough room in the array to insert a new
    FileObjectInfo element, the size of the array gets doubled.
    After the element is inserted, "used" member of the given
    array is postincremented.
*/
void Insert(FileObjectsInfo * array, const char * path, ino_t inode){
    check_and_initialize_array();
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

/*
    Frees the memory used by the given array. This function
    is ment to be called at the end of the program.
*/
void Free(FileObjectsInfo * array){

    for(int i = 0; i < array->used; i++){
        free(array->list[i].path);
        array->list[i].path = NULL;
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
int FindIndex(FileObjectsInfo * array, const char * path){
    int returnValue = -1;
    if(array->size > 0){
        for(int i = 0; i < array->used; i++){
            if(!strcmp(array->list[i].path, path)){
                returnValue = i;
                break;
            }
        }
    } else {
        return returnValue;
    }
}

/*
   Retrieve the FileObjectInfo element at the given index in the
   given array. 
*/
FileObjectInfo Get(FileObjectsInfo * array, int index){
    return array->list[index];
}

//#######################################################################

/*
    The correct way to test for an error is to call dlerror() 
    to clear any old error conditions, then call dlsym(), and 
    then call dlerror() again, saving its return value into a
    variable, and check whether this saved value is not NULL.
    https://linux.die.net/man/3/dlsym
*/
void* dlsym_wrapper(char *original_function){
    
    dlerror();

    void *function_handler;
    
    function_handler = dlsym(RTLD_NEXT, original_function);

    check_dlsym_error();

    return function_handler;
}
/*
    The open wrapper ensures old_open is initialized and is used
    by other inner functions in order to avoid open() recursivity
    and overhead.
*/
int open_wrapper(const char *path, int flags, ...){
    printf("Array size is: %d\n", (int) g_array.size);
    check_and_initialize_array();
    printf("Array size is: %d\n", (int) g_array.size);
    if ( old_open == NULL ) {
        old_open = dlsym_wrapper("open");
    }
    return old_open(path, flags);

}


int __xstat(int ver, const char *path, struct stat *buf)
{

    printf("It is failing here XSTAT ↓↓↓↓↓↓!\n");
    fflush(stdout);
    if ( old_xstat == NULL ) {

        old_xstat = dlsym_wrapper("__xstat");
    }

  //printf("xstat %s\n",path);
    return old_xstat(ver,path, buf);
} 

int __lxstat(int ver, const char *path, struct stat *buf)
{

    int fd, ret;

  // Parenthesis are needed because of operator precedence.
  // https://en.cppreference.com/w/c/language/operator_precedence
    if((fd = open_wrapper(path, O_RDONLY)) < 0){
        printf("Errors occured while trying to access %s.\nAborting.", path);
        perror("Error is: ");
        exit(-1);
    }

    printf("Created fileDescriptor is: %d\n", fd);

    struct stat file_stat;
    if((ret = fstat(fd, &file_stat)) < 0 ){
        printf("Errors occured while trying to stat %d file descriptor.\nAborting.", fd);
        perror("Error is: ");
        exit(-1);
    }

  //After opening a FD, it must be closed
    close(fd);

    ino_t inode = file_stat.st_ino;

    printf("\n#### BEFORE ADDING####\nArray used: %d\n",g_array.used);
    printf("Inode of %s is: %lu\n", path, inode);
    Insert(&g_array, path, inode);
    printf("\n#### AFTER ADDING####\nArray used: %d\n",g_array.used);

  //fileInfo.path = strdup(path);
  //fileInfo.inode = file_stat.st_ino;



  //Adding to global array.
   // printf("##################\nANTES DE ANIADIR:\n");
    //fflush(stdout);

    //printf("##################\nDESPUES DE ANIADIR:\n");
    //fflush(stdout);
    if ( old_lxstat == NULL ) {
        old_lxstat = dlsym_wrapper("__lxstat");
    }

            //printf("Hooked %s whose inode is %d.\n", fileInfo.path, fileInfo.inode);

    return old_lxstat(ver,path, buf);





}

int __xstat64(int ver, const char *path, struct stat64 *buf)
{
    printf("It is failing here XSTAT64 ↓↓↓↓↓↓!\n");
    fflush(stdout);
    if ( old_xstat64 == NULL ) {
        old_xstat64 = dlsym_wrapper("__xstat64");
    }

  //printf("xstat64 %s\n",path);
    return old_xstat64(ver,path, buf);
}

int open(const char *path, int flags, ...)
{

  printf("User invoked open() on: %s\n", path);
  int fileDes = open_wrapper(path, O_RDONLY);
  struct stat fileStat;
  fstat(fileDes, &fileStat);
  int inode = fileStat.st_ino;
  close(fileDes); 

  return open_wrapper(path, flags); 



  
}



//#########################
/*
int
creat64(const char *pathname, mode_t mode)
{
    intercept("creat64", 2);
    set_errno();
    return -1;
}

int
creat(const char *pathname, mode_t mode)
{
    intercept("creat", 2);
    set_errno();
    return -1;
}

int
close(int fd)
{
    intercept("close", 2);
    set_errno();
    return -1;
}

int
open64(const char *pathname, int flags, ...)
{
    intercept("open64", 2);
    set_errno();
    return -1;
}



ssize_t
read(int fd, void *buf, size_t count)
{
    intercept("read", 2);
    set_errno();
    return -1;
}

ssize_t
readv(int fd, const struct iovec *vector, int count)
{
    intercept("readv", 2);
    set_errno();
    return -1;
}

ssize_t
pread(int fd, void *buf, size_t count, unsigned long offset)
{
    intercept("pread", 2);
    set_errno();
    return -1;
}

ssize_t
pread64(int fd, void *buf, size_t count, uint64_t offset)
{
    intercept("pread64", 2);
    set_errno();
    return -1;
}

ssize_t
write(int fd, const void *buf, size_t count)
{
    intercept("write", 2);
    set_errno();
    return -1;
}

ssize_t
writev(int fd, const struct iovec *vector, int count)
{
    intercept("writev", 2);
    set_errno();
    return -1;
}

ssize_t
pwrite(int fd, const void *buf, size_t count, unsigned long offset)
{
    intercept("pwrite", 2);
    set_errno();
    return -1;
}

ssize_t
pwrite64(int fd, const void *buf, size_t count, uint64_t offset)
{
    intercept("pwrite64", 2);
    set_errno();
    return -1;
}

off_t
lseek(int fildes, unsigned long offset, int whence)
{
    intercept("lseek", 2);
    set_errno();
    return -1;
}

off_t
lseek64(int fildes, uint64_t offset, int whence)
{
    intercept("lseek64", 2);
    set_errno();
    return -1;
}

int
dup(int fd)
{
    intercept("dup", 2);
    set_errno();
    return -1;
}

int
dup2(int oldfd, int newfd)
{
    intercept("dup2", 2);
    set_errno();
    return -1;
}

int
mkdir(const char *pathname, mode_t mode)
{
    intercept("mkdir", 2);
    set_errno();
    return -1;
}

int
rmdir(const char *pathname)
{
    intercept("rmdir", 2);
    set_errno();
    return -1;
}

int
chmod(const char *pathname, mode_t mode)
{
    intercept("chmod", 2);
    set_errno();
    return -1;
}

int
chown(const char *pathname, uid_t owner, gid_t group)
{
    intercept("chown", 2);
    set_errno();
    return -1;
}

int
fchmod(int fd, mode_t mode)
{
    intercept("fchmod", 2);
    set_errno();
    return -1;
}

int
fchown(int fd, uid_t uid, gid_t gid)
{
    intercept("fchown", 2);
    set_errno();
    return -1;
}

int
fsync(int fd)
{
    intercept("fsync", 2);
    set_errno();
    return -1;
}

int
ftruncate(int fd, off_t length)
{
    intercept("ftruncate", 1);
    set_errno();
    return -1;
}

int
ftruncate64(int fd, off_t length)
{
    intercept("ftruncate64", 1);
    set_errno();
    return -1;
}

int
link(const char *oldpath, const char *newname)
{
    intercept("link", 2);
    set_errno();
    return -1;
}

int
rename(const char *oldpath, const char *newpath)
{
    intercept("rename", 2);
    set_errno();
    return -1;
}

int
utimes(const char *path, const struct timeval times[2])
{
    intercept("utimes", 2);
    set_errno();
    return -1;
}

int
futimes(int fd, const struct timeval times[2])
{
    intercept("futimes", 2);
    set_errno();
    return -1;
}

int
utime(const char *path, const struct utimbuf *buf)
{
    intercept("utime", 2);
    set_errno();
    return -1;
}

int
mknod(const char *path, mode_t mode, dev_t dev)
{
    intercept("mknod", 2);
    set_errno();
    return -1;
}

int
__xmknod(int ver, const char *path, mode_t mode, dev_t *dev)
{
    intercept("__xmknod", 2);
    set_errno();
    return -1;
}

int
mkfifo(const char *path, mode_t mode)
{
    intercept("mkfifo", 2);
    set_errno();
    return -1;
}

int
unlink(const char *path)
{
    intercept("unlink", 2);
    set_errno();
    return -1;
}

int
symlink(const char *oldpath, const char *newpath)
{
    intercept("symlink", 2);
    set_errno();
    return -1;
}

int
readlink(const char *path, char *buf, size_t bufsize)
{
    intercept("readlink", 1);
    set_errno();
    return -1;
}

char *
realpath(const char *path, char *resolved)
{
    intercept("realpath", 1);
    set_errno();
    return NULL;
}

DIR *
opendir(const char *path)
{
    intercept("opendir", 2);
    set_errno();
    return NULL;
}

struct dirent *
readdir(DIR *dir)
{
    intercept("readdir\t", 2);
    set_errno();
    return NULL;
}

struct dirent *
readdir64(DIR *dir)
{
    intercept("readdir64", 2);
    set_errno();
    return NULL;
}

int
readdir_r(DIR *dir, struct dirent *entry, struct dirent **result)
{
    intercept("readdir_r", 1);
    set_errno();
    return -1;
}

int
readdir64_r(DIR *dir, struct dirent *entry, struct dirent **result)
{
    intercept("readdir64_r", 1);
    set_errno();
    return -1;
}

int
closedir(DIR *dh)
{
    intercept("closedir", 1);
    set_errno();
    return -1;
}

int
stat(const char *path, struct stat *buf)
{
    intercept("stat", 2);
    set_errno();
    return -1;
}

int
stat64(const char *path, struct stat *buf)
{
    intercept("stat64", 2);
    set_errno();
    return -1;
}

int
__fxstat(int ver, int fd, struct stat *buf)
{
    intercept("__fxstat\t", 2);
    set_errno();
    return -1;
}

int
__fxstat64(int ver, int fd, struct stat *buf)
{
    intercept("__fxstat64", 2);
    set_errno();
    return -1;
}


int
fstat(int fd, struct stat *buf)
{
    intercept("fstat", 2);
    set_errno();
    return -1;
}

int
fstat64(int fd, struct stat *buf)
{
    intercept("fstat64", 2);
    set_errno();
    return -1;
}


int
__lxstat64(int ver, const char *path, struct stat *buf)
{
    intercept("__lxstat64", 2);
    set_errno();
    return -1;
}

int
lstat(const char *path, struct stat *buf)
{
    intercept("lstat", 2);
    set_errno();
    return -1;
}

int
lstat64(const char *path, struct stat *buf)
{
    printf("USER INVOKED LSTAT64 ON: %s !!!!! ", path);
    intercept("lstat64", 2);
    set_errno();
    return -1;
}

int
statfs(const char *path, struct statfs *buf)
{
    intercept("statfs", 2);
    set_errno();
    return -1;
}

int
statfs64(const char *path, struct statfs *buf)
{
    intercept("statfs64", 2);
    set_errno();
    return -1;
}

int
statvfs(const char *path, struct statvfs *buf)
{
    intercept("statvfs\t", 2);
    set_errno();
    return -1;
}

int
statvfs64(const char *path, struct statvfs *buf)
{
    intercept("statvfs64", 2);
    set_errno();
    return -1;
}

ssize_t
getxattr(const char *path, const char *name, void *value, size_t size)
{
    intercept("getxattr", 1);
    set_errno();
    return -1;
}

ssize_t
lgetxattr(const char *path, const char *name, void *value, size_t size)
{
    intercept("lgetxattr", 1);
    set_errno();
    return -1;
}

int
remove(const char *path)
{
    intercept("remove", 2);
    set_errno();
    return -1;
}

int
lchown(const char *path, uid_t owner, gid_t group)
{
    intercept("lchown", 2);
    set_errno();
    return -1;
}

void
rewinddir(DIR *dirp)
{
    intercept("rewinddir", 1);
    set_errno();
    return;
}

void
seekdir(DIR *dirp, off_t offset)
{
    intercept("seekdir", 2);
    set_errno();
    return;
}

off_t
telldir(DIR *dirp)
{
    intercept("telldir", 2);
    set_errno();
    return -1;
}

ssize_t
sendfile(int out_fd, int in_fd, off_t *offset, size_t count)
{
    intercept("sendfile\t", 1);
    set_errno();
    return -1;
}

ssize_t
sendfile64(int out_fd, int in_fd, off_t *offset, size_t count)
{
    intercept("sendfile64", 1);
    set_errno();
    return -1;
}

int
fcntl(int fd, int cmd, ...)
{
    intercept("fcntl", 2);
    set_errno();
    return -1;
}*/
