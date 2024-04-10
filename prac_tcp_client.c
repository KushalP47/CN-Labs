#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>
#include<arpa/inet.h>

#define MAX_LINE 256

int main(int argc, char* argv[]){

struct hostent *hp;
struct sockaddr_in sin;
char buf[MAX_LINE];
int s, new_s, len;
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
	printf("Client remote host connected");
}

// clear the sin address
bzero((char*)&sin, sizeof(sin));
// assign the family
sin.sin_family = AF_INET;
// copy the address to sin
bcopy(hp->h_addr, (char*)&sin.sin_addr, hp->h_length);
// assign the server port
sin.sin_port = SERVER_PORT;

// open a socket
if((s = socket(PF_INET, SOCK_STREAM, 0)) < 0){
	perror("simplex-talk: socket");
	exit(1);
}else{
	printf("Client created successfully!!");
}

// connect the socket
if(connect(s, (struct sockaddr *)&sin, sizeof(sin)) < 0){
	perror("simplex-talk: connect");
	close(s);
	exit(1);
}else{
	printf("Connection eshtablished successfully");
}

send(s, "start", sizeof("start"), 0);
printf("Client sent 'start'\n");

recv(s, buf, sizeof(buf), 0);
printf("CLient receive: %s\n", buf);


return 0;
}


