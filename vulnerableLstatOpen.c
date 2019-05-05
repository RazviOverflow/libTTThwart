#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

int process_filename(char *filename)
{
    struct stat aux_stat;
    char buffer[1024];

    printf("Input to be appended: ");
    fgets(buffer, sizeof(buffer), stdin);

    if((lstat(filename, &aux_stat) == 0) && !S_ISLNK(aux_stat.st_mode))
    {
        printf("[+] Opening file %s...\n", filename);
        fflush(stdout);
        int fd, nb;
        if((fd = open(filename, O_RDWR | O_APPEND)) == -1){
            printf("[!] Error while trying to open %s.\n", filename);
            return -1;
        }

        nb = write(fd, buffer, strlen(buffer));
        printf("[+] Done! %d bytes written to %s\n", nb, filename);
        return 0;
    }else
        printf("[-] ERROR: %s is a symlink or does not exist. Exiting...\n", filename);

    return 1;
}


int main(int argc, char * argv[])
{
    if(argc != 2){
        fprintf(stderr, "usage: %s filename\n", argv[0]);
        exit(1);
    }

    return process_filename(argv[1]);
}