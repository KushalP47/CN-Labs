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

#define SERVER_PORT 5433
#define MAX_LINE 256

int main(int argc, char * argv[]){
  FILE *fp;
  struct hostent *hp;
  struct sockaddr_in sin;
  char *host;
  int sp = SERVER_PORT;
  char buf[MAX_LINE];
  int cli, ser;
  int len;
  if (argc==2) {
    host = argv[1];
  }
  else if(argc==3){
    host = argv[1];
    sscanf(argv[2], "%d", &sp);
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
  sin.sin_port = htons(sp);
  
  /* active open */
  if ((cli = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
    perror("simplex-talk: socket");
    exit(1);
  }
  else
    printf("Client created socket.\n");

  if (connect(cli, (struct sockaddr *)&sin, sizeof(sin)) < 0)
    {
      perror("simplex-talk: connect");
      close(cli);
      exit(1);
    }
  else
    printf("Client connected.\n");
  
  /* main loop: get and send lines of text */
  while (1) {
    if(fgets(buf, sizeof(buf), stdin)){ // file writing the data and sending it to server
	    buf[MAX_LINE-1] = '\0';
	    len = strlen(buf) + 1;
	    send(cli, buf, len, 0);
	}else{ // file getting data from the server side
		if ((ser = accept(cli, (struct sockaddr *)&sin, &len)) < 0) {
	      		perror("simplex-talk: accept");
	      		exit(1);
    		}
	    	printf("Client Listening Server Speaking\n");
	    	while (len = recv(ser, buf, sizeof(buf), 0))
	      		fputs(buf, stdout);
	    		close(ser);
		}
  	}
}
