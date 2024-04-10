#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>


#define SERVER_PORT 5432
#define MAX_LINE 256

void recieveFile(int s, FILE* fptr, int fileSize){
    long int packetSize = 256, totalPackets = (fileSize/256);
    if(fileSize%256 != 0)
      totalPackets += 1;
    
    int currentPacket = 0;
    char buf[256];
    printf("Packets Received: %d   Packets Remaining: %d \n", currentPacket, totalPackets);
    while(totalPackets >= currentPacket){
      printf("Packets Received: %d   Packets Remaining: %d \r", currentPacket, totalPackets);
      int l = recv(s, buf, sizeof(buf), 0);
      fwrite(buf, 1, l, fptr);
      currentPacket ++;
    }
    fclose(fptr);
    return;
}

int main(int argc, char * argv[]){
  FILE *fp;
  struct hostent *hp;
  struct sockaddr_in sin;
  char *host, *fileName;
  char buf[MAX_LINE], c;
  int s;
  int len;
  if (argc > 1) {
    // host = argv[1];
    // fileName = argv[3];
    while((c = getopt(argc, argv, "h:f:")) != -1){
        switch(c){
        case 'f':
            fileName = optarg;
            printf("Client's file name: %s\n", fileName);
            break;
        case 'h':
            host = optarg;
            printf("Client connected to server: %s\n", host);
            break;
        case '?':
            fprintf(stderr, "usage: %s -p port host\n", argv[0]);
            exit(1);
        }
    }
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
  
  send(s, fileName, 256, 0);
  printf("Client sent: %s file request\n", fileName);
  recv(s, buf, sizeof(buf), 0);
  printf("file size %s", buf);
  int fileSize = atoi(buf);
  printf("File size to recieve: %d \n", fileSize);
  if(fileSize == -1){
    printf("File doesn't exists\n");
    close(s);
    return 0;
  }

  char path[MAX_LINE] = "./responses/";
  strcat(path, fileName);
  printf("%s", path);
  FILE* fptr = fopen(path, "wb");
  printf("File size to recieve: %d \n", fileSize);
  recieveFile(s, fptr, fileSize);
  printf("File Received\n");
  close(s);

}