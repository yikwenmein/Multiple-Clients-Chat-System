#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <regex.h>

char msg[500];
char proto[15];
char nick[15];

void *recvmg(void *my_sock)
{
    int sock = *((int *)my_sock);
    int len;
    // client thread always ready to receive message
    while((len = recv(sock,msg,500,0)) > 0) {
        msg[len] = '\0';
        fputs(msg,stdout);
    }
}
int match(const char *string, const char *pattern) 
{ 
    regex_t re; 
    if (regcomp(&re, pattern, REG_EXTENDED|REG_NOSUB) != 0) return 0; 
    int status = regexec(&re, string, 0, NULL, 0); 
    regfree(&re); 
    if (status != 0) return 0; 
    return 1; 
} 
int main(int argc,char *argv[]){

    if(argc != 3)
        {
            printf("Error: two arguments are needed!\n");
            exit(1);
          }
       char delim[]=":";
       char *Desthost=strtok(argv[1],delim);
       char *Destport=strtok(NULL,delim);
       int port = atoi(Destport);
      if(strlen(argv[2])<12){
                       
                         
       	const char* re = "^[A-Za-z0-9_]+$";         
       if (match(argv[2],re))
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
   




    pthread_t recvt;
    int len;
    int sock;
    char send_msg[500];
    struct sockaddr_in server_addr;
    char nick_name[100];
    strcpy(nick_name, argv[2]);
    sock = socket( AF_INET, SOCK_STREAM,0);

    // assign IP, PORT 
	server_addr.sin_family = AF_INET; 
	server_addr.sin_addr.s_addr = inet_addr(Desthost);  
	server_addr.sin_port = htons(port); 

    if( (connect( sock ,(struct sockaddr *)&server_addr,sizeof(server_addr))) == -1 )
        printf("\n connection to socket failed \n");
     
    printf("Connected to server on %s:%d\n", inet_ntoa(server_addr.sin_addr) , ntohs(server_addr.sin_port));
     bzero(proto,sizeof(proto) ); 
       int ret = read(sock, proto, sizeof(proto)); //recvfrom(sockfd, buffer, BUF_SIZE, 0, NULL, NULL);  
       if (ret<0) {  
       printf("Error receiving data!\n");
          
       } else {
       proto[ret] = '\0';
       printf("Server protocol: %s\n",proto);
       }
        if (strncmp(proto, "HELLO 1",7) ==0){
       printf("Protocol supported, sending Nickname\n");
       }
     bzero(nick,sizeof(nick));
     strcpy(nick,argv[2]);
     strcat(nick,"\n");

     ret = write(sock,nick,strlen(nick));
     if (ret<0) {  
       perror(" Failed on write() \n");
     }
     bzero(proto,sizeof(proto) ); 
       ret = read(sock, proto, sizeof(proto)); //recvfrom(sockfd, buffer, BUF_SIZE, 0, NULL, NULL);  
       if (ret<0) {  
       perror("Failed on read()\n");
          
       } else {
       proto[ret] = '\0';
       printf("Received: %s\n",proto);
       if (strncmp(proto, "OK",2) ==0){
       printf("Name Accepted\n");
       }
      
       }

    //creating a client thread which is always waiting for a message
    pthread_create(&recvt,NULL,(void *)recvmg,&sock);

    //ready to read a message from console
    while(fgets(msg,500,stdin) > 0) {
        strcpy(send_msg,nick_name);
        strcat(send_msg,":");
        strcat(send_msg,msg);
        

        if(strlen(send_msg)>=255){
            printf("messages are limited to 255 characters\n");
            exit(1);
            }


        len = write(sock,send_msg,strlen(send_msg));
        if(len < 0) 
            printf("\n message not sent \n");
    }

    //thread is closed
    pthread_join(recvt,NULL);
    close(sock);
    return 0;
}