#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define DELAY 50000

int main(int argc, char * argv[]) {
  char * filename, buffer[60];
  int i, a;
  FILE * file;
  if (argc != 2) {
    fprintf(stderr, "usage: %s filename\n", argv[0]);
    exit(1);
  }
  filename = argv[1];
  printf("Insert your input:\n");
  scanf(" %50s", buffer);
  // get user input 
  if (!access(filename, W_OK)) {
    for (i = 0; i < DELAY; i++) // simulate delay 
      a = i * i;
    // Open the file 
    file = fopen(filename, "a+"); // Write to file the user input
    fwrite(buffer, sizeof(char), strlen(buffer), file);
    fwrite("\n", sizeof(char), 2, file);
    fclose(file);
    printf("Writing done\n");
  } else printf("No permission , exiting !\n");
  return 0;
}