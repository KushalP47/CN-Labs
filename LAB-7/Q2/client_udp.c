#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

// Now you can use struct hostent
struct hostent *he;
#define SERVER_PORT 5432
#define MAX_LINE 256

void receiveFile(int s, FILE* fptr, int fileSize) {
    long int packetSize = 256, totalPackets = (fileSize/256);
    if(fileSize % 256 != 0)
        totalPackets += 1;

    int currentPacket = 0, flag = 0;
    char buf[256];
    while(totalPackets >= currentPacket){
        printf("Packets Received: %d   Packets Remaining: %ld \r", currentPacket, totalPackets-currentPacket);
        int l = recv(s, buf, sizeof(buf), 0);
        fwrite(buf, 1, l, fptr);
        if(currentPacket > totalPackets/100 && flag == 0){
            flag = 1;
            
        }
        currentPacket++;
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
    socklen_t len; // Change to socklen_t
    if (argc > 1) {
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
    if ((s = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("simplex-talk: socket");
        exit(1);
    }
    else
        printf("Client created socket.\n");

    sendto(s, fileName, strlen(fileName), 0, (struct sockaddr *)&sin, sizeof(sin));
    printf("Client sent: %s file request\n", fileName);

    len = sizeof(sin); // Correct assignment
    if ((recvfrom(s, buf, sizeof(buf), 0, (struct sockaddr *)&sin, &len)) < 0) {
        perror("simplex-talk: recvfrom");
        exit(1);
    }

    printf("File size: %s\n", buf);
    int fileSize = atoi(buf);
    printf("File size to receive: %d bytes\n", fileSize);

    if(fileSize == -1){
        printf("File doesn't exist.\n");
        close(s);
        return 0;
    }

    char path[MAX_LINE] = "./responses/";
    strcat(path, fileName);
    printf("Saving file to: %s\n", path);
    FILE* fptr = fopen(path, "wb");
    if (!fptr) {
        perror("Failed to open file");
        close(s);
        exit(1);
    }

    receiveFile(s, fptr, fileSize);
    printf("File Received.\n");
    close(s);

    return 0;
}
