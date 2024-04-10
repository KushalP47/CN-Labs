#include <stdio.h>
#include<stdlib.h>
#include <unistd.h>
#include <string.h>
void readFile(char fileName[], char buffer[]){
      
      char c;
      FILE *fd;
      fd = fopen (fileName, "r");
      if( fd == NULL ){
            printf("File not found. Exiting.\n");
            exit(EXIT_FAILURE);
     }
      /* Read src until end-of-file char is encountered */
      fread(buffer, 1, 100, fd);
    //   printf("\n");
      fclose(fd);
    
};

void writeFile(char fileName[], char buffer[]){
    
    char c;
    FILE *fd;
    fd = fopen (fileName, "w");
    if( fd == NULL ){
            printf("File not found. Exiting.\n");
            exit(EXIT_FAILURE);
     }
    // printf("Enter text to write to file: ");
    fwrite(buffer, 1, strlen(buffer), fd);
    // printf("\n");
    fclose(fd);
};

int main(int argc, char **argv){
     
    char fileName[100] = "", buffer[100] = "";
    char c;
    while( (c = getopt(argc, argv, "i:o:")) != -1){
        switch(c){
            case 'i':
                strcpy(fileName, optarg);
                readFile(fileName, buffer);
                break;
            case 'o':
                strcpy(fileName, optarg);
                writeFile(fileName, buffer);
                break;
            default:
                strcpy(fileName, "sample.txt");
                readFile(fileName, buffer);
        }
    }
    
    
    return 0;
}