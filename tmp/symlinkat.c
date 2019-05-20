#define _GNU_SOURCE

#include <sys/types.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <string.h>

int main(){

	int directory_fd = dirfd(opendir("/tmp/"));

	int fd = open("testing_symlinkat.txt", O_WRONLY|O_CREAT|O_TRUNC, 0666);
	close(fd);

	fd = open("/tmp/testing_symlinkat2.txt", O_WRONLY|O_CREAT|O_TRUNC, 0666);
	close(fd);

	fd = open("file_SYMLINKAT2", O_WRONLY|O_CREAT|O_TRUNC, 0666);
	close(fd);

	// Symlinkat takes both arguments *oldpath and *newpath relatives to newdirfd

	char old_aux_path[strlen(get_current_dir_name()) + strlen("testing_symlinkat.txt") + 1];
	char new_aux_path[strlen(get_current_dir_name()) + strlen("file_SYMLINKAT1") + 1];
	char new_aux_path4[strlen(get_current_dir_name()) + strlen("file_SYMLINKAT4") + 1];
	char new_aux_path3[strlen(get_current_dir_name()) + strlen("file_SYMLINKAT3") + 1];
	char new_aux_path5[strlen(get_current_dir_name()) + strlen("file_SYMLINKAT5") + 1];

	// +1 because of  in-between '/'
	snprintf(old_aux_path, sizeof(old_aux_path)+1, "%s/%s", get_current_dir_name(), "testing_symlinkat.txt");
	snprintf(new_aux_path, sizeof(new_aux_path)+1, "%s/%s", get_current_dir_name(), "file_SYMLINKAT1");
	snprintf(new_aux_path4, sizeof(new_aux_path)+1, "%s/%s", get_current_dir_name(), "file_SYMLINKAT4");
	snprintf(new_aux_path3, sizeof(new_aux_path)+1, "%s/%s", get_current_dir_name(), "file_SYMLINKAT3");
	snprintf(new_aux_path5, sizeof(new_aux_path)+1, "%s/%s", get_current_dir_name(), "file_SYMLINKAT5");

	//oldpath is absolute and newpath is relative
	symlinkat(old_aux_path, directory_fd, "file_SYMLINKAT");
	symlinkat("/tmp/file_SYMLINKAT", directory_fd, "file_SYMLINKAT20");
	symlinkat("/tmp/temporal/testing_symlinkat2.txt", directory_fd, "file_SYMLINKAT21");
	symlinkat("/nonexistent", directory_fd, "file_SYMLINKAT22");
	symlinkat("/tmp/temporal/testing_symlinkat2.txt", AT_FDCWD, "./temporal/file_SYMLINKAT21");
	symlinkat("file_SYMLINKAT2", AT_FDCWD, "./temporal/file_SYMLINKAT22");
	symlinkat("../file_SYMLINKAT2", AT_FDCWD, "./temporal/file_SYMLINKAT23");
	symlinkat("./../file_SYMLINKAT2", AT_FDCWD, "./temporal/file_SYMLINKAT24");
	symlinkat(".././file_SYMLINKAT2", AT_FDCWD, "./temporal/file_SYMLINKAT25");

	//oldpath is relative and newpath is relative
	symlinkat("testing_symlinkat2.txt", directory_fd, "file_SYMLINKAT2");
	symlinkat("/temporal/testing_symlinkat2.txt", directory_fd, "file_SYMLINKAT10");
	symlinkat("/testing_symlinkat2.txt", directory_fd, "file_SYMLINKAT11");
	symlinkat("../testing_symlinkat2.txt", directory_fd, "file_SYMLINKAT12");
	symlinkat("./temporal/testing_symlinkat2.txt", directory_fd, "file_SYMLINKAT13");

	//oldpath is relative and newpath is absolute
	symlinkat("file_SYMLINKAT2", directory_fd, new_aux_path);
	symlinkat("/tmp/testing_symlinkat2.txt", directory_fd, new_aux_path4);
	symlinkat("../testing_symlinkat3.txt", directory_fd, new_aux_path3);
	symlinkat("./temporal/testing_symlinkat3.txt", directory_fd, new_aux_path5);

	//oldpath is absolute and newpath is absolute
	symlinkat(new_aux_path, directory_fd, "/tmp/testing_symlinkat4.txt");


	return 0;
}