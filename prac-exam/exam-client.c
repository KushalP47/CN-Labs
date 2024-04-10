/*
Computer Networks, Lab exam practice
Date: 27/03/24
*/

/* FILL THIS BEFORE PROCEEDING
Name: Kushal Patel
Roll number: AU2140105
IP address: 127.0.0.1
*/

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

int main(int argc, char * argv[]){
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
   /*  Code to handle command line arguments. 
       the first argument must be the remote IP address
       and the second argument must be the remote port number.
       
       if both the arguments are not present, print an error
       message and exit.
    */
  
  // END OF THE COMMAND LINE PARSING
  
  // For the prctice exam, you can start with the following...
  if (argc==3) {
    host = argv[1];
    SERVER_PORT = atoi(argv[2]);
  }
  else {
    fprintf(stderr, "usage: client host port\n");
    exit(1);
  }
  
  /* translate host name into peer's IP address */
  hp = gethostbyname(host);
  if (!hp) {
    /* print unknown host error message and exit */
    fprintf(stderr, "%s: unknown host %s\n", argv[0], host);
  }else{
    printf("Client's remote host: %s\n", host);
  }

  /* build address data structure */
  
  /* Add code to build address data structure sin*/
  /* Add code to zero out the data structure memory */
  /* Set address family to AF_INET*/
  
  // = AF_INET;

  /* Set destination IP address */
  //bcopy(hp->h_addr, (char *)&sin.sin_addr, hp->h_length);
  
  /* Set destination port */
  // sin.sin_port = htons(server_port);
  

  /* The following fragment implements the above four lines*/
  /* It will not be given in the exam. You will have to write yourself. */	
  bzero((char *)&sin, sizeof(sin));
  sin.sin_family = AF_INET;
  bcopy(hp->h_addr, (char *)&sin.sin_addr, hp->h_length);
  sin.sin_port = htons(SERVER_PORT);
  
  
  
  /* Open a TCP socket and assign handle new_s
     check for error; print message and exit if error
     if socket is successfully created, print message
     confirming the same.
     
     if socket is successfully created, connect to the
     remote host and print message if successful
     
     If connect fails, close the socket, print error message and exit.
    
     Function calls hints:
     socket(int socket_family, int socket_type, int protocol);
     
     int connect(int sockfd, const struct sockaddr *addr,
                   socklen_t addrlen)
   */
  if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0 ){
        perror("simplex-talk: socket");
        exit(1);
    }
    else{
        printf("Client created socket.\n");
  }

  if(connect(s, (struct sockaddr *)&sin, sizeof(sin)) < 0){
    perror("simplex-talk: connect");
    close(s);
    exit(1);
  }else{
    printf("Connection established successfully\n");
  }
 

  /* 1. Implement the following protocol. 
     2. Print all sent and received messages on screen.
     3. Your grades will be sent by the test server.
     4. You can assume that messages from server are 
     	at-most 256 bytes at a time
     
     Message exchange sequence to be implemented after open()
     --------------------------------------------------------
	
     send "start"
     receive and print message from server
     send integer 4567890 
     receive and print server message
     send "bye"
     receive and print message from server
     close the socket
  */
  
send(s, "start", sizeof("start"), 0);
printf("Client send start\n");

recv(s, buf, sizeof(buf), 0);
printf("Server send: %s\n", buf);
int num = 4567890;
int con = htonl(num);
send(s, &con, sizeof(con), 0);
printf("Client send 4567890\n");
recv(s, buf, sizeof(buf), 0);
printf("Server send: %s\n", buf);

send(s, "bye", sizeof("bye"), 0);
printf("Client send bye\n");
recv(s, buf, sizeof(buf), 0);
printf("Server send: %s\n", buf);
close(s);
  
  return 0;
}