#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

int main(){

	char *filename = "temporalFile";

	remove(filename);
    creat(filename, S_IRUSR | S_IWUSR);


	return 0;
}