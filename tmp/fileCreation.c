// RazviOverflow

#define _GNU_SOURCE

#include <sys/types.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <string.h>

/*
	Simple C program to test out every single way of changing the inode of a
	given path. That is, creating and deleting file. The main purpose of this
	file is to analize it with strace and ltrace in order to see what system
	calls are actually invoked when the devolper calls these glibc functions. 

	The set of functions tested is inspired by [1], [2]:

	[1] http://profile.iiita.ac.in/bibhas.ghoshal/lab_files/System%20calls%20for%20files%20and%20directories%20in%20Linux.html
	[2] http://linasm.sourceforge.net/docs/syscalls/filesystem.php

	The present code creates the following files in the Current Working Directory (CWD):
	- file_CREAT.txt
	- file_LINK_HARDLINK (hard link to file_CREAT.txt)
	- file_MKNOD.txt
	- file_OPEN.txt
	- file_SYMLINK (symbolic link to file_CREAT.txt)

	It creates the following files in /tmp directory:
	- file_LINKAT_HARDLINK (hard link to file_CREAT.txt)
	- file_MKNODAT.txt
	- file_OPENAT.txt
	- file_SYMLINKAT (symbolic link to file_CREAT.txt)

	Then it sleeps for 10 seconds and deletes all of the created files using different functions
	like unlink, unlinkat and remove.
*/
int main(){
	
	int fd, contentLength = 40;
	/*
		System call OPEN
		This function returns the file descriptor or in case of an error -1.
		The number of arguments that this function can have is two or three.
		The third argument is used only when creating a new file
	*/

	fd = open("file_OPEN.txt", O_WRONLY|O_CREAT|O_TRUNC, 0666);
	write(fd, "File created with OPEN function\n", contentLength);
	close(fd);

	remove("file_OPEN.txt");

	fd = open("file_OPEN1.txt", O_WRONLY|O_CREAT|O_TRUNC, 0666);
	write(fd, "File created with OPEN function\n", contentLength);
	close(fd);
	remove("file_OPEN1.txt");
	fd = open("file_OPEN2.txt", O_WRONLY|O_CREAT|O_TRUNC, 0666);
	write(fd, "File created with OPEN function\n", contentLength);
	close(fd);
	fd = open("file_OPEN3.txt", O_WRONLY|O_CREAT|O_TRUNC, 0666);
	write(fd, "File created with OPEN function\n", contentLength);
	close(fd);

	remove("file_OPEN2.txt");
	remove("file_OPEN3.txt");

	int directory_fd = dirfd(opendir("/tmp/"));


	fd = openat(directory_fd, "file_OPENAT.txt", O_WRONLY|O_CREAT|O_TRUNC, 0666);
	write(fd, "File created with OPENAT function\n", contentLength);

	// -------

	/*
		System call MKNOD
		The system call mknod() creates a filesystem node (file, device
       	special file, or named pipe) named pathname, with attributes
       	specified by mode and dev.
	*/

	mknod("file_MKNOD.txt", S_IFREG, 0);

	mknodat(directory_fd, "file_MKNODAT.txt", S_IFREG, 0);

	close(fd);

	// -------

	/*
		System call CREAT
		The function call: creat(pathname,mode) is equivalent to the call:
		open(pathname, O_CREAT|O_WRONLY|O_TRUNC, mode);
		Thus the file named by pathname is created, unless it already exists.
		The file is then opened for writing only, and is truncated to zero length
	*/ 

	
	fd = creat("file_CREAT.txt", S_IRUSR | S_IWUSR);
	write(fd, "File created with CREAT function\n", contentLength);
	close(fd);

	// -------

	/*	
		System call LINK && LINKAT
		To link an existing file to another directory (or to the same
		directory) link can be used. 
		The link system call creates a hard link. Creating symbolic links can
		be done using symlink system call.
	*/



	link("file_CREAT.txt", "file_LINK_HARDLINK");

	linkat(AT_FDCWD, "file_CREAT.txt", directory_fd, "file_LINKAT_HARDLINK", AT_SYMLINK_FOLLOW) ;

	// -------

	/*	
		System call SYMLINK && SYMLINKAT
		To link an existing file to another directory (or to the same
		directory) link can be used. 
		The link system call creates a hard link. Creating symbolic links can
		be done using symlink system call.
	*/

	symlink("file_CREAT.txt","file_SYMLINK");

	// Symlinkat takes both arguments *oldpath and *newpath relatives to newdirfd

	char aux_path[strlen(get_current_dir_name()) + strlen("file_CREAT.txt") + 1];

	// +1 because of  in-between '/'
	snprintf(aux_path, sizeof(aux_path)+1, "%s/%s", get_current_dir_name(), "file_CREAT.txt");

	symlinkat(aux_path, directory_fd, "file_SYMLINKAT");


	sleep(5);


	// -------

	/*
		System call UNLINK && UNLINK AT
		To delete a link (a path) in a directory we can use the unlink system call.
		The function decrements the hard link counter in the i-node and
		deletes the appropriate directory entry for the file whose link was
		deleted. If the number of links of a file becomes 0 then the space
		occupied by the file and its i-node will be freed
	*/

	unlink("file_OPEN.txt");
	unlink("file_MKNOD.txt");
	unlink("file_SYMLINK");

	unlinkat(directory_fd, "file_SYMLINKAT", 0);
	unlinkat(directory_fd, "file_LINKAT_HARDLINK", 0);
	unlinkat(directory_fd, "///////file_SYMLINKAT", 0);
	unlinkat(directory_fd, "../file_LINKAT_HARDLINK", 0);

	// -------

	// REMOVE
	remove("/tmp/file_OPENAT.txt");
	remove("file_CREAT.txt");
	remove("/tmp/file_MKNODAT.txt");
	remove("file_LINK_HARDLINK");

	return 0;
}