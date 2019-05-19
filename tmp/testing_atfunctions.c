// RazviOverflow

#define _GNU_SOURCE

#include <sys/types.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>

/*
	File to test all 3 possibilites of *at functions. 
*/
int main(){
	
	int fd, contentLength = 50;

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
	char *file = "file_OPENAT.txt";
	char *file_in_parent = "../file_OPENAT2.txt";
	char *file_creating_subfolder = "./tmp2/file_OPENAT3.txt";
	char *file_to_be_sanitized = ".///.////.//file_OPENAT4.txt";
	char *file_to_be_sanitized2 = "///./././tmp/./file_OPENAT5.txt";

	printf("Call 1\n");
	fd = openat(directory_fd, file, O_WRONLY|O_CREAT|O_TRUNC, 0666);
	write(fd, "File created with OPENAT relative function\n", contentLength);
	if(fd < 0){
		printf("Error %s\n", strerror(errno));
	}

	printf("Call 2\n");
	fd = openat(AT_FDCWD, file, O_WRONLY|O_CREAT|O_TRUNC, 0666);
	write(fd, "File created with OPENAT at_fdcwd function\n", contentLength);
	if(fd < 0){
		printf("Error %s\n", strerror(errno));
	}

	printf("Call 3\n");
	fd = openat(AT_FDCWD, file_creating_subfolder, O_WRONLY|O_CREAT|O_TRUNC, 0666);
	write(fd, "File created with OPENAT at_fdcwd ./tmp3/ function\n", contentLength);
	if(fd < 0){
		printf("Error %s\n", strerror(errno));
	}

	printf("Call 4\n");
	fd = openat(AT_FDCWD, file_in_parent, O_WRONLY|O_CREAT|O_TRUNC, 0666);
	write(fd, "File created with OPENAT at_fdcwd ../ function\n", contentLength);
	if(fd < 0){
		printf("Error %s\n", strerror(errno));
	}

	printf("Call 5\n");
	fd = openat(directory_fd, file_creating_subfolder, O_WRONLY|O_CREAT|O_TRUNC, 0666);
	write(fd, "File created with OPENAT at_fdcwd directory_fd/tmp3/ function\n", contentLength);
	if(fd < 0){
		printf("Error %s\n", strerror(errno));
	}

	printf("Call 6\n");
	fd = openat(directory_fd, file_in_parent, O_WRONLY|O_CREAT|O_TRUNC, 0666);
	write(fd, "File created with OPENAT at_fdcwd ../directory_fd function\n", contentLength);
	if(fd < 0){
		printf("Error %s\n", strerror(errno));
	}

	
	char aux[strlen(get_current_dir_name()) + strlen("file_OPENAT2.txt") + 1];
	snprintf(aux, sizeof(aux)+1, "%s/%s", get_current_dir_name(), "file_OPENAT2.txt");
	
	printf("Call 7\n");
	fd = openat(directory_fd, aux, O_WRONLY|O_CREAT|O_TRUNC, 0666);
	write(fd, "File created with OPENAT absolute function\n", contentLength);
	if(fd < 0){
		printf("Error %s\n", strerror(errno));
	}

	printf("Call 8\n");
	fd = openat(AT_FDCWD, file_to_be_sanitized, O_WRONLY|O_CREAT|O_TRUNC, 0666);
	write(fd, "File created with OPENAT at_fdcwd ../ function\n", contentLength);
	if(fd < 0){
		printf("Error %s\n", strerror(errno));
	}

	printf("Call 9\n");
	fd = openat(directory_fd, file_to_be_sanitized2, O_WRONLY|O_CREAT|O_TRUNC, 0666);
	write(fd, "File created with OPENAT at_fdcwd ../ function\n", contentLength);
	if(fd < 0){
		printf("Error %s\n", strerror(errno));
	}

	// -------

	fflush(stdout);
	sleep(5);


	unlinkat(directory_fd, file_to_be_sanitized2, 0);
	unlinkat(directory_fd, file_creating_subfolder, 0);
	remove(file);
	remove(file_creating_subfolder);
	remove(file_in_parent);
	remove(file_to_be_sanitized);
	unlink("file_OPENAT2.txt");
	unlinkat(directory_fd, file, 0);
	unlinkat(directory_fd, file_in_parent, 0);
	unlinkat(directory_fd, file_to_be_sanitized2, 0);
	unlinkat(directory_fd, file_creating_subfolder, 0);


	close(fd);
	close(directory_fd);

	

	return 0;
}