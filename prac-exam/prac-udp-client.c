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

#define MAX_LINE 256

int main(int argc, char *argv[]){
    FILE fp;
    struct hostent *hp;
    struct sockaddr_in sin;
    char buf[MAX_LINE];
    int s, len;
    socklen_t l;
    char *host;
    unsigned short SERVER_PORT;

    if(argc == 3){
        host = argv[1];
        SERVER_PORT = atoi(argv[2]);
    }else{
        fprintf(stderr, "usage: client host port");
        exit(1);
    }

    hp = gethostbyname(host);
    if(!hp){
        fprintf(stderr, "usage: client host name");
        exit(1);
    }else{
        print("CLient remote host connected");
    }

    bzero((char *)&sin, sizeof(sin));
    sin.sin_family = AF_INET;
    bcopy(hp->h_addr, (char *)&sin.sin_addr, hp->h_length);
    sin.sin_port = htons(SERVER_PORT);

    if((s = socket(PF_INET, SOCK_DGRAM, 0)) < 0){
        perror("simplex-talk: socket");
        exit(1);
    }else{
        printf("Socket created successfully");
    }

    sendto(s, "hello", sizeof("hello"), 0,(struct sockaddr*)&sin, sizeof(sin));
    printf("Client send Hello\n");

    recvfrom(s, buf, sizeof(buf), 0, (struct sockaddr *)&sin, sizeof(sin));
    printf("Server send %s", buf);

    
    return 0;
}