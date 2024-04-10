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
// #define SERVER_PORT 5432
#define MAX_LINE 4096

void receiveFile(int s, FILE* fptr) {
    char buf[4096];
    int bytesReceived = 0;
    while (1) {
        bzero(buf, sizeof(buf));
        bytesReceived = recv(s, buf, sizeof(buf), 0);
        if (bytesReceived < 0) {
            perror("Failed to receive file");
            fclose(fptr);
            close(s);
            exit(1);
        }
        if (bytesReceived == 0) {
            break;
        }
        if(strcmp(buf, "BYE") == 0){
            break;
        }
        fwrite(buf, 1, bytesReceived, fptr);
    }
    fclose(fptr);
    return;
}

int main(int argc, char * argv[]){
    FILE *fp;
    struct hostent *hp;
    struct sockaddr_in sin;
    char *host, fileName[] = "response.mp4";
    char buf[MAX_LINE], c;
    int s, SERVER_PORT;
    socklen_t len; // Change to socklen_t
    if (argc > 1) {
        while((c = getopt(argc, argv, "h:p:")) != -1){
            switch(c){
                case 'h':
                    host = optarg;
                    printf("Client connected to server: %s\n", host);
                    break;
                case 'p':
                    SERVER_PORT = atoi(optarg);
                    printf("Client connected to port: %d\n", SERVER_PORT);
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
    // fileName = "GET";
    sendto(s, "GET", 4, 0, (struct sockaddr *)&sin, sizeof(sin));
    printf("Client sent: %s file request\n", fileName);
    len = sizeof(sin); // Correct assignment
    if ((recvfrom(s, buf, sizeof(buf), 0, (struct sockaddr *)&sin, &len)) < 0) {
        perror("simplex-talk: recvfrom");
        exit(1);
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
    receiveFile(s, fptr);
    printf("File Received.\n");
    close(s);
    return 0;
}
