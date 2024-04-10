#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<strings.h>
#include<netdb.h>

#define MAX_LINE 256

int main(int argc, char* argv[]){

struct hostent *hp;
struct sockaddr_in sin;
char *host;
unsigned short SERVER_PORT;
int s;
char buf[MAX_LINE];
socklen_t len;

if(argc == 3){
	host = argv[1];
	SERVER_PORT = atoi(argv[2]);
}else{
	fprintf(stderr, "usage: client host port\n");
	exit(1);
}

hp = gethostbyname(host);
if(!hp){
	fprintf(stderr, "%s : unknown host: %s \n", argv[0], host);
}else{
	printf("Client's remote host: %s\n", host);
}

bzero((char*)&sin, sizeof(sin));
sin.sin_family = AF_INET;
bcopy(hp->h_addr, (char *)&sin.sin_addr, hp->h_length);
sin.sin_port = SERVER_PORT; 

if((s = socket(PF_INET, SOCK_DGRAM, 0)) < 0){
	perror("simplex-talk: socket");
	exit(1);
}else{
	printf("Client created socket\n");
}

sendto(s, "hello", sizeof("hello"), 0, (struct sockaddr *)&sin, sizeof(sin));
printf("Client send hello\n");
len=sizeof(sin);
recvfrom(s, buf, sizeof(buf), 0, (struct sockaddr *)&sin, &len);
printf("Server send %s\n", buf);

return 0;
}

