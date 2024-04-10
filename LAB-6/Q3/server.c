#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SERVER_PORT 5432
#define MAX_LINE 256

int findSize(char *file_name) 
{ 
    printf("Server got %s file request", file_name);
    // opening the file in read mode 
    FILE* fp = fopen(file_name, "r"); 
  
    // checking if the file exists or not 
    if (fp == NULL) { 
        // printf("File Not Found!\n"); 
        return -1; 
    } 
  
    fseek(fp, 0L, SEEK_END); 
  
    // calculating the size of the file 
    long int res = ftell(fp); 
  
    // closing the file 
    fclose(fp); 
  
    return res; 
} 

void sendFile(int s, struct sockaddr *client_addr, socklen_t client_len, char *fileName, long int fileSize) {
    int packetSize = 256, totalPackets = (fileSize/256);
    if(fileSize % 256 != 0)
        totalPackets++;

    int currentPacket = 0;
    char buf[256];
    FILE* fptr = fopen(fileName, "rb");
    printf("%d %d\n", currentPacket, totalPackets);
    while(totalPackets >= currentPacket){
        printf("Packets Send: %d Packets Remaining: %ld \r", currentPacket, totalPackets);
        fflush(stdout);
        int re = fread(buf, 1, sizeof(buf), fptr);
        sendto(s, buf, re, 0, client_addr, client_len);
        udelay(300);
        currentPacket++;
    }
    printf("\nFile Sent\n");
    fclose(fptr);
}

int main(){

    struct sockaddr_in sin, client_addr;
    char buf[MAX_LINE], *fileName;
    socklen_t len = sizeof(client_addr);
    int s;
    char str[INET_ADDRSTRLEN];

    /* build address data structure */
    bzero((char *)&sin, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;
    sin.sin_port = htons(SERVER_PORT);

    /* setup passive open */
    if ((s = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
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

    while(1){
        printf("Server Listening.\n");

        if ((recvfrom(s, buf, sizeof(buf), 0, (struct sockaddr *)&client_addr, &len)) < 0) {
            perror("simplex-talk: recvfrom");
            exit(1);
        }

        fileName = buf;
        printf("Server received: %s file request \n", fileName);
        int fileSize = findSize(fileName);
        char str[3];
        sprintf(str, "%d", fileSize);
        printf("%d %s\n", fileSize, str);
        sendto(s, str, strlen(str), 0, (struct sockaddr *)&client_addr, len);
        if(fileSize == -1)
            continue;
        sendFile(s, (struct sockaddr *)&client_addr, len, fileName, fileSize);
    }

    close(s);
}
