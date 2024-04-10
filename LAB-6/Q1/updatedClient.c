#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

// function which takes new created File, fileSize && socket id as params
void recieveFile(int s, FILE* fptr, int fileSize){

    // getting the packets info
    long int packetSize = 256, totalPackets = (fileSize/256);
    if(fileSize%256 != 0)
      totalPackets += 1;
    
    int currentPacket = 0;

    // creating an empty buffer where packets data will get stored
    char buf[256];
    while(totalPackets >= currentPacket){

      // the \r helps to rewrite the statement again and again
      printf("Packets Send: %d   Packets Remaining: %d \r", currentPacket, totalPackets);

      // recieving the packets
      int l = recv(s, buf, sizeof(buf), 0);
      fwrite(buf, 1, l, fptr); // writing in the file(binary)
      currentPacket ++;
    }
    fclose(fptr);
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
  int s, SERVER_PORT = 5432, flag = 1;
  int len;
  if (argc == 3) {
    host = argv[1];
    SERVER_PORT = atoi(argv[2]);
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

  
  // After getting connected we are sending "GET" request to server
  send(s, "GET", 3, 0);
  // confirmation msg
  printf("Client sent: GET file request\n");

  // recieving the reply from server
  recv(s, buf, sizeof(buf), 0);
  printf("file size %s", buf);

  // we will recieve the file Size of the requested file first if its -1 then the file don't exist in server
  int fileSize = atoi(buf);
  printf("File size to recieve: %d \n", fileSize);
  if(buf == "-1"){
    printf("File doesn't exists\n");
    close(s);
    return 0;
  }

  // now that we know file exists, we will create the file in client with same name and will write in binary in it
  FILE* fptr = fopen("client_sample.txt", "wb");
  printf("File size to recieve: %d \n", fileSize);

  // recieveFile() will get all the data from server
  recieveFile(s, fptr, fileSize);
  printf("File Received\n");
  close(s);
  
}