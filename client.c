
#include <stdio.h>  
#include <stdlib.h>  
#include <sys/types.h>  
#include <sys/socket.h>  
#include <string.h>  
#include <netinet/in.h> 
#include <netdb.h>
#include <fcntl.h>
#include <errno.h>
#include <regex.h>

  
//#define PORT 4711
#define BUF_SIZE 2000
#define SA struct sockaddr 
int match(const char *string, const char *pattern) 
{ 
    regex_t re; 
    if (regcomp(&re, pattern, REG_EXTENDED|REG_NOSUB) != 0) return 0; 
    int status = regexec(&re, string, 0, NULL, 0); 
    regfree(&re); 
    if (status != 0) return 0; 
    return 1; 
} 
  
int main(int argc, char**argv) {  

        int not_done = 1;
	int sockfd, connfd, port, ret; 
	char string;
	char buffer[BUF_SIZE];  
	struct sockaddr_in servaddr, cli; 
	struct hostent *server;
	const char* re = "^[A-Za-z0-9_]+$";
	 // Check and set program arguments
       if(argc != 3)
        {
            printf("Error: two arguments are needed!\n");
            exit(1);
          }
       char delim[]=":";
       char *Desthost=strtok(argv[1],delim);
       char *Destport=strtok(NULL,delim);
       port = atoi(Destport);
      if(strlen(argv[2])<12){
                       
                         
                
       if (match(argv[2],re) && strlen(argv[2])<=12)
                {
              printf("%s is a valid nickname.\n",argv[2]);
          

                } 
                
                else {
                perror("Nickname is not a valid nickname.\n");
                
                //close(new_socket);
                exit(1);
                } 
            }
            else {
            printf("Nicknames are limited to 12 characters\n");
            exit(1);
            }
	sockfd = socket(AF_INET, SOCK_STREAM, 0); 
	if (sockfd == -1) { 
		printf("socket creation failed...\n"); 
		exit(1); 
	} 
	else
		printf("Socket successfully created..\n"); 
	bzero(&servaddr, sizeof(servaddr)); 

	servaddr.sin_family = AF_INET; 
	servaddr.sin_addr.s_addr = inet_addr(Desthost); 

	servaddr.sin_port = htons(port); 
	// connect the client socket to server socket 
	if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) { 
		printf("connection with the server failed...\n"); 
		exit(0); 
	} 
	else
       printf("connected to the server..\n"); 

       bzero(buffer,sizeof(buffer) ); 
       ret = read(sockfd, buffer, sizeof(buffer)); //recvfrom(sockfd, buffer, BUF_SIZE, 0, NULL, NULL);  
       if (ret<0) {  
       printf("Error receiving data!\n");
       close(sockfd);
       exit(1);    
       } else {
       printf("Received after connecting to server: %s\n",buffer);
       //fputs(buffer, stdout);
       //printf("\n");
       }
       char msg[20] ="NICK";// Vicky\n";
       strcat(msg, " "); 
       strcat(msg,argv[2]);
       strcat(msg,"\n");
      //printf("String to server is: %s\n", argv[2]);
       
       ret = write(sockfd, msg, strlen(msg)); 
        if (ret<0) {  
            printf("Error sending data!\n\t-%s", buffer);
            close(sockfd);
            exit(1);   
        }
       printf("Sent to server:%s\n", msg); 
       
       bzero(buffer,sizeof(buffer) ); 
       
       
       
       ret = read(sockfd, buffer, sizeof(buffer)); //recvfrom(sockfd, buffer, BUF_SIZE, 0, NULL, NULL);  
       if (ret<0) {  
       printf("Error receiving data!\n"); 
       close(sockfd);
       exit(1);    
       } else {
      
       
       printf("Second Responds from server: %s\n",buffer);
       
     
     
    }
      
 while(not_done){
printf("Enter your message:" );
       fflush(stdout);
char lineBuffer[4096];
fd_set master;
fd_set readfds;

FD_ZERO(&master);
FD_ZERO(&readfds);

FD_SET(0,&master);
FD_SET(sockfd,&master); // s is a socket descriptor
       readfds = master;
       
        if (select(sockfd+1, &readfds, NULL, NULL, NULL)<0){
          printf("select() call has failed\n");
          }
        if (FD_ISSET(sockfd, &readfds)){
        
        if ((ret = read( sockfd , buffer, 1024)) == 0)
	   {
	printf("\nServer disconnected\n");
	not_done = 0;
	close(sockfd);
        exit(1);

	
	}
        
        
        
        ret = read(sockfd, buffer, sizeof(buffer));//recvfrom(sockfd, buffer, BUF_SIZE, 0, NULL, NULL);  
       if (ret<0) {  
       printf("Error receiving data!\n");
       not_done = 0;  
       close(sockfd);
       exit(1);  
       }
        
        }
        
      
       if (FD_ISSET(0, &readfds)){
      
       int len = read(0, lineBuffer, 4096);
      
         lineBuffer[strlen(lineBuffer)-1] = '\0';
       if (match(lineBuffer,re) && strlen(lineBuffer)<=255)
                {
              printf("%s is a valid message.\n",lineBuffer);
          

                } 
                
                else {
                perror("message is not a valid nickname.\n");
               
                //close(new_socket);
                exit(1);
                } 
       char message[5000] ="MSG";
       //strcat(message, "MSG");
       strcat(message, " "); 
       strcat(message,lineBuffer);
       bzero(lineBuffer,sizeof(lineBuffer) );
       strcat(message,"\n");
       
       //printf("%s\n", message);
       
       
       ret = write(sockfd, message, strlen(message));//sendto(sockfd, buffer, BUF_SIZE, 0, (struct sockaddr *) &addr, sizeof(addr));  
       if (ret<0) {  
       printf("Error sending data!\n\t-%s", buffer);  
       }
        printf("Message to server is: %s\n", message);
       
      bzero(message,sizeof(message) );
       //free(lineBuffer);
       bzero(buffer,sizeof(buffer) ); 
        ret = read(sockfd, buffer, sizeof(buffer));//recvfrom(sockfd, buffer, BUF_SIZE, 0, NULL, NULL);  
       if (ret<0) {  
       printf("Error receiving data!\n");
       not_done=0;
       close(sockfd);
       exit(1);    
       }
       else {
       printf("Received: %s\n",buffer);
       bzero(buffer,sizeof(buffer) ); 

     }  
  
     }
     }
close(sockfd); 
}
