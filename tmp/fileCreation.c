
#include <sys/types.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>


int main(){

	struct stat aux_stat;
	FILE *fp;

	fp=fopen("file_FOPEN.txt","w");
	fprintf(fp, "Razvan");
	fclose(fp);

	remove("file_FOPEN.txt");

	int fd=open("OPEN_file.txt", O_WRONLY|O_CREAT|O_TRUNC, 0666);
	
	close(fd);

	stat("OPEN_file.txt", &aux_stat);
	unlink("OPEN_file.txt");

	return 0;
}