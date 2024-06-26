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

#define SERVER_PORT 5432
#define MAX_PENDING 5
#define MAX_LINE 256

// function to find the size of file
int findSize(char file_name[]) 
{ 
    // opening the file in read mode 
    FILE* fp = fopen(file_name, "r"); 
  
    // checking if the file exist or not 
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

// function to send the file to client (if it exists)
void sendFile(int s, char fileName[], long int fileSize){

	// getting the packets info
	int packetSize = 256, totalPackets = (fileSize/256);
	if(fileSize%256 != 0)
		totalPackets++;
	
	int currentPacket = 0;

	// creating an empty buffer where packets data will get stored
	char buf[256];
	FILE* fptr = fopen(fileName, "rb");
	printf("%d %d\n", currentPacket, totalPackets);
	while(totalPackets >= currentPacket){

		// the \r helps to rewrite the statement again and again
		printf("Packets Send: %d Packets Remaining: %ld \r", currentPacket, totalPackets);
		fflush(stdout);

		// reading the data and storing it in a packet
		int re = fread(buf, 1, sizeof(buf), fptr);
		send(s, buf, re, 0); // sending the packet
		
		currentPacket++;
	}
	printf("\nFile Sent\n");
	fclose(fptr);
	return;
}

int main(){

	struct sockaddr_in sin;
	char buf[MAX_LINE], fileName[] = "server_sample.txt";
	socklen_t len;
	int s, new_s;
	char str[INET_ADDRSTRLEN];

	/* build address data structure */
	bzero((char *)&sin, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = INADDR_ANY;
	sin.sin_port = htons(SERVER_PORT);
	
	/* setup passive open */
	if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
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

	listen(s, MAX_PENDING);
	
	while(1){ // server started running

			
			if ((new_s = accept(s, (struct sockaddr *)&sin, &len)) < 0) {
				perror("simplex-talk: accept");
				exit(1);
			}
			
			printf("Server Listening.\n");
			// if a new client gets connected the inner loop will start
			while(len = recv(new_s, buf, sizeof(buf), 0)){

					// check the type of the request
					if(strcmp(buf, "GET") == 0){
							printf("Server received: %s file request \n", buf);
							// getting the size of the file
							int fileSize = findSize(fileName);
							char str[3];
							sprintf(str, "%d", fileSize);
							printf("%d %s\n", fileSize, str);
							send(new_s, str, strlen(str), 0);
							// stop communication if file doesn't exists
							if(fileSize == -1){
								close(new_s);
								break;
							}

							// sendFile()
							sendFile(new_s, fileName, fileSize);
							close(new_s);
							break;
					}else{
							printf("Server received incorrect request: %s\n", buf);
							close(new_s);
							break;
					}
			}
	}

}
