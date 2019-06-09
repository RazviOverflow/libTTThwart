#define _GNU_SOURCE

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h> 
#include <stdio.h> 
#include <utime.h>
#include <sys/time.h>

/*
	This is just a toy file to see what library calls
	are actually made (with ltrace).
*/

int main(){

	char *file = "file_OPEN.txt";
	int fd;

	fd = open(file, O_WRONLY|O_CREAT|O_TRUNC, 0666);
	write(fd, "File created with OPEN function\n", 50);
	close(fd);


	symlink(file,"file_SYMLINK");

	symlinkat(file, AT_FDCWD, "file_SYMLINKAT");


	link(file, "file_LINK_HARDLINK");

	linkat(AT_FDCWD, file, AT_FDCWD, "file_LINKAT_HARDLINK", AT_SYMLINK_FOLLOW) ;

	rename("file_SYMLINK", "file_SYMLINKrenamed");
	renameat(AT_FDCWD, "file_SYMLINKrenamed", AT_FDCWD, "file_SYMLINKrenamed2");
	//renameat2(AT_FDCWD, "file_SYMLINKrenamed", AT_FDCWD, "file_SYMLINKrenamed2", 1);

	fd = creat("file_CREAT.txt", S_IRUSR | S_IWUSR);
	write(fd, "File created with CREAT function\n", 50);
	close(fd);

	fd = creat64("file_CREAT64.txt", S_IRUSR | S_IWUSR);
	write(fd, "File created with CREAT function\n", 50);
	close(fd);

	fd = open("file_OPEN.txt", O_WRONLY|O_CREAT|O_TRUNC, 0666);
	write(fd, "File created with OPEN function\n", 50);
	close(fd);


	fd = open64("file_OPEN64.txt", O_WRONLY|O_CREAT|O_TRUNC, 0666);
	write(fd, "File created with OPEN function\n", 50);
	close(fd);

	fd = openat(AT_FDCWD, "file_OPENAT.txt", O_WRONLY|O_CREAT|O_TRUNC, 0666);
	write(fd, "File created with OPENAT function\n", 50);
	close(fd);

	FILE *stream = fopen(file, "r");
	fclose(stream);

	stream = fopen64("file_CREAT64.txt", "r");
	fclose(stream);

	fd = open("file_OPEN.txt", O_WRONLY|O_CREAT|O_TRUNC, 0666);
	stream = fdopen(fd, "r");
	
	close(fd);
	//fclose(stream);
	

	stream = popen("echo popen", "r");
	pclose(stream);	

	mknod("file_MKNOD.txt", S_IFREG, 0);

	mknodat(AT_FDCWD, "file_MKNODAT.txt", S_IFREG, 0);

	mkfifo("file_MKFIFO.txt", 0777);
	mkfifoat(AT_FDCWD, "file_MKFIFOAT.txt", 0777);

	chmod("file_MKFIFO.txt", S_IROTH);
	chown("file_MKFIFO.txt", -1, -1);

	truncate("file_CREAT.txt", 1);
	truncate64("file_CREAT64.txt", 1);

	struct utimbuf time; 
	time.modtime = 0;

	utime("file_CREAT.txt", &time);

	struct timeval times[2];

	utimes("file_CREAT.txt", &times);

	pathconf(file, "_PC_LINK_MAX");

	mkdir("tmpmkdir", 0777);
	mkdirat(AT_FDCWD, "tmpmkdirat", 0777);

	//mount

	chdir("tmpmkdir");
	chroot("..");
	//pivot_root("tmpmkdir", "..");


	return 0;
}