/* 
   1. Understand the following code.
   2. Print a character n times where the character and n are 
      passed as command-line arguments.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void usage(void);

int main (int argc, char **argv){
  int num = 2;
  char disp = '*';
  char c;

  while ((c = getopt (argc, argv, "n:d:h:")) != -1)
    {
        // printf("c = %c\n", c);
        switch (c){
        
        case 'n':
        // printf("optarg = %s\n", optarg);
        num = atoi(optarg);
        break;
        
        case 'd':
            // printf("optarg = %s\n", optarg);
            disp = optarg[0];
            break;
        case 'h':
          printf("%s\n", optarg);
          break;
          
        default:
        usage();
        exit (1);
        }
    }
  printf ("number = %d, display char = %c\n",
          num, disp);
  
  for (int i = 0; i < num; i++)
    printf ("%c ", disp);
  printf ("\n");
  return 0;
}

void usage(void){

  printf("Usage:\n");
  printf(" -n <repetitions>\n");
  printf(" -d <char to dispaly>\n");
  exit (8);
}