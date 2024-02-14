#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

void createFile(int s, char buf[]){
  FILE *fp;
  // char buf[256];
  int len;
  fp = fopen("client_sample.txt", "w");
  if (fp == NULL) {
    perror("File does not exists.");
    exit(1);
  }
  // if(recv(s, buf, sizeof(buf), 0) != 0){
    printf("Server sent: %s\n", buf);
    fputs(buf, fp);
  // }
  fclose(fp);
  printf("File received.\n");
  return;
}

// #define SERVER_PORT 5432
#define MAX_LINE 256

int main(int argc, char * argv[]){
  FILE *fp;
  struct hostent *hp;
  struct sockaddr_in sin;
  char *host;
  char buf[MAX_LINE], *fileName, c;
  int s, SERVER_PORT = 5432, flag = 0;
  int len;
  if (argc == 3) {
    host = argv[1];
    SERVER_PORT = atoi(argv[2]);
  }
  else if(argc == 4){
    host = argv[1];
    SERVER_PORT = atoi(argv[2]);
    fileName = argv[3];
    flag = 1;
  }
  else {
    fprintf(stderr, "usage: %s host\n", argv[0]);
    exit(1);
  }
  /* translate host name into peer's IP address */
  hp = gethostbyname(host);
  if (!hp) {
    fprintf(stderr, "%s: unknown host: %s\n", argv[0], host);
    exit(1);
  }
  else
    printf("Client's remote host: %s\n", argv[1]);
  
  /* build address data structure */
  bzero((char *)&sin, sizeof(sin));
  sin.sin_family = AF_INET;
  bcopy(hp->h_addr, (char *)&sin.sin_addr, hp->h_length);
  sin.sin_port = htons(SERVER_PORT);
  
  /* active open */
  if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
    perror("simplex-talk: socket");
    exit(1);
  }
  else
    printf("Client created socket.\n");

  if (connect(s, (struct sockaddr *)&sin, sizeof(sin)) < 0)
    {
      perror("simplex-talk: connect");
      close(s);
      exit(1);
    }
  else
    printf("Client connected.\n");
  
  if(flag == 1){  // send file request to server
    send(s, "GET", 3, 0);
    printf("Client sent: GET file request\n");
    recv(s, buf, sizeof(buf), 0);
    createFile(s, buf);
    printf("%s\n", buf);
    close(s);
  }else{  // take input from user and send to server
    while (fgets(buf, sizeof(buf), stdin)) {
      buf[MAX_LINE-1] = '\0';
      len = strlen(buf) + 1;
      send(s, buf, len, 0);
    }
  }
}