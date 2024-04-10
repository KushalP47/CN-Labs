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
#define MAX_PENDING 5
#define MAX_LINE 256

int main(int argc, char * argv[]){
  FILE *fp;
  struct sockaddr_in sin;
  char buf[MAX_LINE];
  socklen_t len;
  int ser, cli, lenn;
int sp = SERVER_PORT;
  char str[INET_ADDRSTRLEN];
  if(argc == 2){
   sscanf(argv[1], "%d", &sp);
  }

  /* build address data structure */
  bzero((char *)&sin, sizeof(sin));
  sin.sin_family = AF_INET;
  sin.sin_addr.s_addr = INADDR_ANY;
  sin.sin_port = htons(sp);
  
  /* setup passive open */
  if ((ser = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
    perror("simplex-talk: socket");
    exit(1);
  }
 
  inet_ntop(AF_INET, &(sin.sin_addr), str, INET_ADDRSTRLEN);
  printf("Server is using address %s and port %d.\n", str, sp);

  if ((bind(ser, (struct sockaddr *)&sin, sizeof(sin))) < 0) {
    perror("simplex-talk: bind");
    exit(1);
  }
  else
    printf("Server bind done.\n");

  listen(ser, MAX_PENDING);
  
  /* wait for connection, then receive and print text */
  while(1) {
	if(fgets(buf, sizeof(buf), stdin)){
		    buf[MAX_LINE-1] = '\0';
		    lenn = strlen(buf) + 1;
		    send(ser, buf, lenn, 0);
	}
	else{
	    if ((cli = accept(ser, (struct sockaddr *)&sin, &len)) < 0) {
	      perror("simplex-talk: accept");
	      exit(1);
	    }
	    printf("Server Listening  Client Writing\n");

	    while (lenn = recv(cli, buf, sizeof(buf), 0))
	      fputs(buf, stdout);
	    close(cli);
	  }
	}
}

