    
//Example code: A simple server side code, which echos back the received message.
//Handle multiple socket connections with select and fd_set on Linux
#include <stdio.h>
#include <string.h> //strlen
#include <stdlib.h>
#include <errno.h>
#include <unistd.h> //close
#include <arpa/inet.h> //close
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros
#include <regex.h>
	
#define TRUE 1
#define FALSE 0
#define PORT 5010
#define MAX_STRING_SIZE 40

int match(const char *string, const char *pattern) 
{ 
    regex_t re; 
    if (regcomp(&re, pattern, REG_EXTENDED|REG_NOSUB) != 0) return 0; 
    int status = regexec(&re, string, 0, NULL, 0); 
    regfree(&re); 
    if (status != 0) return 0; 
    return 1; 
} 

int main(int argc , char *argv[])
{       
        
        
        
    char delim[]=":";
	char *Desthost=strtok(argv[1],delim);
        char *Destport=strtok(NULL,delim);
        int port = atoi(Destport);
        int state = TRUE;
	int opt = TRUE;
	int master_socket , addrlen , new_socket , client_socket[30] ,
		max_clients = 30 , activity, i , valread , sd;
	int max_sd;
	char last[1024];
	bzero(last, sizeof(last));
	char nickname[15], nick[6];
	char nicknames[max_clients][50];
	struct sockaddr_in address;
		
	char buffer[1025]; //data buffer of 1K
	const char* re = "^[A-Za-z0-9_]+$"; 
	//char *first = NULL;
	//char *last;	
	//set of socket descriptors
	fd_set readfds;
		
	//a message
	char *message = "HELLO 1.0\n";
	
	//initialise all client_socket[] to 0 so not checked
	for (i = 0; i < max_clients; i++)
	{
		client_socket[i] = 0;
	}
		
	//create a master socket
	if( (master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0)
	{
		perror("socket failed");
		exit(EXIT_FAILURE);
	}
	
	//set master socket to allow multiple connections ,
	//this is just a good habit, it will work without this
	if( setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt,
		sizeof(opt)) < 0 )
	{
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}

	
	
	// assign IP, PORT 
	address.sin_family = AF_INET; 
	
	address.sin_addr.s_addr = inet_addr(Desthost);

	address.sin_port = htons( port);
		
	//bind the socket to localhost port 8888
	if (bind(master_socket, (struct sockaddr *)&address, sizeof(address))<0)
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
	printf("Listener on port %d \n", port);
		
	//try to specify maximum of 3 pending connections for the master socket
	if (listen(master_socket, 3) < 0)
	{
		perror("listen");
		exit(EXIT_FAILURE);
	}
		
	//accept the incoming connection
	addrlen = sizeof(address);
	puts("Waiting for connections ...");
		
	while(TRUE)
	{
		//clear the socket set
		FD_ZERO(&readfds);
	
		//add master socket to set
		FD_SET(master_socket, &readfds);
		max_sd = master_socket;
			
		//add child sockets to set
		for ( i = 0 ; i < max_clients ; i++)
		{
			//socket descriptor
			sd = client_socket[i];
				
			//if valid socket descriptor then add to read list
			if(sd > 0)
				FD_SET( sd , &readfds);
				
			//highest file descriptor number, need it for the select function
			if(sd > max_sd)
				max_sd = sd;
		}
	
		//wait for an activity on one of the sockets , timeout is NULL ,
		//so wait indefinitely
		activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);
	
		if ((activity < 0) && (errno!=EINTR))
		{
			printf("select error\n");
		}
			
		//If something happened on the master socket ,
		//then its an incoming connection
		if (FD_ISSET(master_socket, &readfds))
		{
			if ((new_socket = accept(master_socket,
					(struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
			{
				perror("accept");
				exit(EXIT_FAILURE);
			}
			
			//inform user of socket number - used in send and receive commands
			printf("New connection , socket fd is %d , ip is : %s , port : %d\n"
				, new_socket , inet_ntoa(address.sin_addr) , ntohs
				(address.sin_port));
		
			//send new connection greeting message
			if( send(new_socket, message, strlen(message), 0) != strlen(message) )
			{
				perror("send");
			}
				
			puts("Welcome message sent successfully");
			if ((valread = read( new_socket , buffer, 1024)) > 0)
				{
			getpeername(new_socket , (struct sockaddr*)&address , \
						(socklen_t*)&addrlen);
			
			
			if (strncmp(buffer, "NICK", 4) ==0){
			
			
			int rv = sscanf(buffer, "%s %s", nick, nickname);
			
			printf("Nickname is: %s\n",nickname);
			
  
                        printf("Testing nicknames. \n");
  
                        // for(int i=2;i<argc;i++){
                        if(strlen(nickname)<12){
                       
                         
                        
						if (match(nickname,re))
                        {
	                   printf("%s is a valid nickname.\n",nickname);
	                
	
                        } 
                        
                       else {
                       perror("Nickname is not a valid nickname.\n");
	              
	               
	                send(new_socket, "ERROR bad nick (len)\n", strlen("ERROR bad nick (len)\n"), 0); 
	                close(new_socket);
	                
	
                       } 
			}
			else {
			printf("Nicknames are limited to 12 characters\n");
	        
			send(new_socket, "ERROR bad nick (len)\n", strlen("ERROR bad nick (len)\n"), 0);
			 close(new_socket);
			}
			
			
			
			bzero(buffer,sizeof(buffer));
			
			send(new_socket, "OK\n", strlen("OK\n"), 0);
			}
			else if (strncmp(buffer, "NICK", 4) !=0){
			send(new_socket, "ERROR malformed command\n", strlen("ERROR malformed command\n"), 0);
			//state = FALSE;
			close(new_socket);
			//strcpy(nickname,"None");
			}
			
			}	
			//add new socket to array of sockets
			for (i = 0; i < max_clients; i++)
			{
				//if position is empty
				if( client_socket[i] == 0 )
				{
					client_socket[i] = new_socket;
					
					printf("Adding to list of sockets as %d\n" , i);
					strcpy(nicknames[i], nickname);
					//printf("nick: %s\n",nicknames[i]);
						
					break;
				}
			}
		}
			
		//else its some IO operation on some other socket
		for (i = 0; i < max_clients; i++)
		{
			sd = client_socket[i];
				
			if (FD_ISSET( sd , &readfds))
			{
				//Check if it was for closing , and also read the
				//incoming message
				bzero(buffer,sizeof(buffer));
				if ((valread = read( sd , buffer, 1024)) == 0)
				{
				

					getpeername(sd , (struct sockaddr*)&address , \
						(socklen_t*)&addrlen);
					printf("Client %s disconnected, port %d \n" ,
						inet_ntoa(address.sin_addr) , ntohs(address.sin_port));
						
					//Close the socket and mark as 0 in list for reuse
					
					close( sd );
					//memset(nicknames, 0, 30*50*sizeof nicknames[0][0]);
					client_socket[i] = 0;
					
				}
					
				//Echo back the message that came in
				else
				{
                                        
                                        getpeername(sd , (struct sockaddr*)&address , \
						(socklen_t*)&addrlen);
					buffer[valread] = '\0';

				       if (strncmp(buffer, "MSG", 3) ==0 ){

				       char msg[256];
				       bzero(msg,sizeof(msg));
				       char *copy = strchr(buffer, ' ');
                                       if (copy != NULL) {
                                       copy++;
                                       strcat(last,"MSG ");
                                       strcat(last,nicknames[i]);
                                  
                                       strcat(last," ");
									   strcpy(msg, copy);
									   msg[strlen(msg)-1]='\0';
						              if (match(msg,re)&& strlen(msg)<=255)
											{

											} 
											else {
												printf("The  message is not valid\n");
												send(sd, "ERROR bad message\n", strlen("ERROR bad message\n"), 0); 
												close(sd);
											}
                                       strcat(last,msg);
                                       //free(copy);
                                       }
					                
					send(sd , last , strlen(last) , 0 );
					bzero(buffer, sizeof(buffer));
					bzero(last, sizeof(last));
					
				       }
				       else {
				       send(sd, "ERROR malformed command\n", strlen("ERROR malformed command\n"), 0);
				       close(sd);
				       } 
				       
				}
			}
		}
	}
		
	return 0;
}


