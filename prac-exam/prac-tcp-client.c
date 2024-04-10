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
    FILE *fp;
    struct hostent *hp;
    struct sockaddr_in sin;
    char *host;
    char buf[MAX_LINE];
    int s, new_s;
    unsigned short SERVER_PORT;
    char c;
    int len;
    socklen_t l;

    if(argc == 3){
        host = argv[1];
        SERVER_PORT = atoi(argv[2]);
    }else{
        fprintf(stderr, "usage client host port\n");
        exit(1);
    }

    // translate host's name into peer's IP address
    hp = gethostbyname(host);
    if(!hp){
        fprintf(stderr, "%s: unknown host %s\n", argv[0], host);
        ecit(1);
    }else{
        printf("Client's remote host: %s\n", host);
    }

    // build addres data structure
    /* Add code to build address data structure sin*/
    /* Add code to zero out the data structure memory */
    bzero((char*)&sin, sizeof(sin));
    /* Set address family to AF_INET*/
    sin.sin_family = AF_INET;
    /* Set destination IP address */
    bcopy(hp->h_addr, (char *)&sin.sin_addr, hp->h_length);
    // set dest port
    sin.sin_port = htons(SERVER_PORT);

    // open a socket
    if((s = socket(PF_INET, SOCK_STREAM, 0)) < 0){
        perror("simplex-talk: socket");
        exit(1);
    }else{
        printf("CLient created socket\n");
    }

    // connect the socket
    if(connect(s, (struct sockaddr *)&sin, sizeof(sin)) < 0){
        perror("simplex-talk: connect");
        close(s);
        exit(1);
    }else{
        printf("Connection established successfully");
    }

    send(s, "start", sizeof("start"), 0);
    print("CLient send start\n");

    recv(s, buf, sizeof(buf), 0);
    print("Server send: %s", buf);

    return 0;
}