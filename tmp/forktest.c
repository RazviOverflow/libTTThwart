#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>


int main(int argc, char * argv[])
{

    mkdir("d1", S_IRWXU|S_IRWXG|S_IRWXO);
    mkdir("d2", S_IRWXU|S_IRWXG|S_IRWXO);
    mkdir("d3", S_IRWXU|S_IRWXG|S_IRWXO);
    mkdir("d4", S_IRWXU|S_IRWXG|S_IRWXO);

    struct stat aux_stat;
    stat("d1", &aux_stat);
    sleep(2);

    if(!fork()){
            // Child
            stat("d1", &aux_stat);

        } else {
            // Parent
            wait(NULL);

            
        }
        return 0;


    return 0;
}