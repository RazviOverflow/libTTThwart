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
	File to test all 3 possibilites of *at functions. 
*/
int main(){
	
	int fd, contentLength = 100;

	int directory_fd = dirfd(opendir("/tmp/"));

	/*
		OPENAT

		int openat(int dirfd, const char *pathname, int flags);
		int openat(int dirfd, const char *pathname, int flags, mode_t mode);

		If the pathname given in pathname is relative, then it is interpreted 
		relative to the directory referred to by the file descriptor dirfd 
		(rather than relative to the current working directory of the calling 
		process, as is done by open(2) for a relative pathname).

		If pathname is relative and dirfd is the special value AT_FDCWD, 
		then pathname is interpreted relative to the current working directory 
		of the calling process (like open(2)).

		If pathname is absolute, then dirfd is ignored.

	*/

	fd = openat(directory_fd, "file_OPENAT.txt", O_WRONLY|O_CREAT|O_TRUNC, 0666);
	write(fd, "File created with OPENAT relative function\n", contentLength);

	fd = openat(AT_FDCWD, "file_OPENAT.txt", O_WRONLY|O_CREAT|O_TRUNC, 0666);
	write(fd, "File created with OPENAT at_fdcwd function\n", contentLength);

	char aux[strlen(get_current_dir_name()) + strlen("file_OPENAT2.txt") + 1];
	snprintf(aux, sizeof(aux)+1, "%s/%s", get_current_dir_name(), "file_OPENAT2.txt");


	fd = openat(directory_fd, aux, O_WRONLY|O_CREAT|O_TRUNC, 0666);
	write(fd, "File created with OPENAT absolute function\n", contentLength);

	// -------


	close(fd);
	close(directory_fd);

	

	return 0;
}