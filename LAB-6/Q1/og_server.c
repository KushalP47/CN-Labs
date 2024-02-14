#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#define SERVER_PORT 5432
#define MAX_PENDING 5
#define MAX_LINE 256

void sendFile(int s, char *fileName){
  FILE *fp;
  char buf[MAX_LINE];
  int len;
  printf("Server sending file: %s\n", fileName);
  fp = fopen(fileName, "r");
  if (fp == NULL) {
    perror("File does not exists.");
    exit(1);
  }
  while (fgets(buf, sizeof(buf), fp)) {
    buf[MAX_LINE-1] = '\0';
    len = strlen(buf) + 1;
    send(s, buf, len, 0);
  }
  fclose(fp);
}

int main(){

  struct sockaddr_in sin;
  char buf[MAX_LINE];
  socklen_t len;
  int s, new_s;
  char str[INET_ADDRSTRLEN];

  /* build address data structure */
  bzero((char *)&sin, sizeof(sin));
  sin.sin_family = AF_INET;
  sin.sin_addr.s_addr = INADDR_ANY;
  sin.sin_port = htons(SERVER_PORT);
  
  /* setup passive open */
  if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
    perror("simplex-talk: socket");
    exit(1);
  }
 
  inet_ntop(AF_INET, &(sin.sin_addr), str, INET_ADDRSTRLEN);
  printf("Server is using address %s and port %d.\n", str, SERVER_PORT);

  if ((bind(s, (struct sockaddr *)&sin, sizeof(sin))) < 0) {
    perror("simplex-talk: bind");
    exit(1);
  }
  else
    printf("Server bind done.\n");

  listen(s, MAX_PENDING);
  
  while(1){
    if ((new_s = accept(s, (struct sockaddr *)&sin, &len)) < 0) {
      perror("simplex-talk: accept");
      exit(1);
    }
    printf("Server Listening.\n");
    while(len = recv(new_s, buf, sizeof(buf), 0)){
      if(strcmp(buf, "GET") == 0){
        printf("Server received: %s file request \n", buf);
        char buff[] = "sample.txt";
        sendFile(new_s, buff);
        close(new_s);
        break;
      }else{
        while (len = recv(new_s, buf, sizeof(buf), 0))
          fputs(buf, stdout);
        close(new_s);
      }
    }
  }
  // while(1) {
  //   if ((new_s = accept(s, (struct sockaddr *)&sin, &len)) < 0) {
  //     perror("simplex-talk: accept");
  //     exit(1);
  //   }
  //   printf("Server Listening.\n");
  //   while (len = recv(new_s, buf, sizeof(buf), 0))
  //     fputs(buf, stdout);
  //   close(new_s);
  // }
}
