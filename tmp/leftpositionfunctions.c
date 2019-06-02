 #include <sys/types.h>
 #include <sys/stat.h>
 #include <unistd.h>
 #include <fcntl.h> 
 #include <stdio.h> 

/*
	This is just a toy file to see what library calls
	are actually made (with ltrace).
*/

int main(){

	struct stat *statStruct;

	stat("README.md", statStruct);
	lstat("README.md", statStruct);
	access("README.md", R_OK);
	mkdir("tmpdir", S_IRUSR | S_IWUSR);
	rmdir("tmpdir");

	char *file = "tmpfile.txt";
	int fd = open(file, O_WRONLY|O_CREAT|O_TRUNC, 0666);
	write(fd, "File created with OPENAT relative function\n", 50);

	unlink(file);

	write(fd, "File created with OPENAT relative function\n", 50);

	unlinkat(AT_FDCWD, file, 0);

	write(fd, "File created with OPENAT relative function\n", 50);

	remove(file);

	write(fd, "File created with OPENAT relative function\n", 50);

	symlink(file, "tmpLink");	

	char buf[10];

	readlink(file, buf, sizeof(buf));
	readlinkat(AT_FDCWD, file, buf, sizeof(buf));

	remove("tmpLink");


	return 0;
}

