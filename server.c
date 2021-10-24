#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <regex.h>
pthread_mutex_t mutex;
int clients[20], ret;
int n=0;


int match(const char *string, const char *pattern) 
{ 
    regex_t re; 
    if (regcomp(&re, pattern, REG_EXTENDED|REG_NOSUB) != 0) return 0; 
    int status = regexec(&re, string, 0, NULL, 0); 
    regfree(&re); 
    if (status != 0) return 0; 
    return 1; 
} 

void sendtoall(char *msg,int curr){
    int i;
    pthread_mutex_lock(&mutex);
    for(i = 0; i < n; i++) {
        if(clients[i] != curr) {
            //MSG_NOSIGNAL flag prevents a SIGPIPE signal if client closed the connection
            if(send(clients[i],msg,strlen(msg),MSG_NOSIGNAL) < 0) {
                //printf("sending failure \n");
                continue;
            }
        }
    }
    pthread_mutex_unlock(&mutex);
}

void *recvmg(void *client_sock){
    int sock = *((int *)client_sock);
    char msg[500];
    int len;
    while((len = recv(sock,msg,500,0)) > 0) {
        msg[len] = '\0';
        
        sendtoall(msg,sock);
    }
  if(recv(sock,msg,500,0)==0){
    printf("client left\n");
    int clnts = n;
    clnts--;
    printf("clients: %d\n",clnts);
  }
}

int main(int argc,char *argv[]){
    char delim[]=":";
	char *Desthost=strtok(argv[1],delim);
    char *Destport=strtok(NULL,delim);
    int port = atoi(Destport);
    //int state = TRUE;
	//int opt = TRUE;
	//int master_socket , addrlen , new_socket , client_socket[30] ,
		//max_clients = 30 , activity, i , valread , sd;
	int max_sd, addrlen;
	char last[1024];
	bzero(last, sizeof(last));
	char nickname[15], nick[6];
	char nicknames[20][15];
	struct sockaddr_in address;
		
	char buffer[1025]; //data buffer of 1K
	const char* re = "^[A-Za-z0-9_]+$"; 
    const char* rem = "^[a-zA-Z0-9_ ]+$";
    char *message = "HELLO 1";

    //struct sockaddr_in ServerIp;
    pthread_t recvt;
    int sock=0 , Client_sock=0;

    // assign IP, PORT 
	address.sin_family = AF_INET; 
	
	address.sin_addr.s_addr = inet_addr(Desthost);

	address.sin_port = htons( port);
    sock = socket( AF_INET , SOCK_STREAM, 0 );

    int yes=1;
// lose the pesky "Address already in use" error message
if (setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof yes) == -1) {
    perror("setsockopt");
    exit(1);
} 


    if( bind( sock, (struct sockaddr *)&address, sizeof(address)) == -1 )
        perror("cannot bind, error!!");
    else
        printf("Listening for connections . . . \n");

    if( listen( sock ,20 ) == -1 )
        printf("listening failed \n");
    addrlen = sizeof(address);
    while(1){
        if( (Client_sock = accept(sock, (struct sockaddr *)&address,(socklen_t*)&addrlen)) < 0 )
            printf("accept failed  \n");
        printf("New connection on %s:%d\n"
				, inet_ntoa(address.sin_addr) , ntohs
				(address.sin_port));
		
			//send new connection greeting message
			if( send(Client_sock, message, strlen(message), 0) != strlen(message) )
			{
				perror("send error");
			}

             ret = read(Client_sock, nickname, sizeof(nickname)); //recvfrom(sockfd, buffer, BUF_SIZE, 0, NULL, NULL);  
       if (ret<0) {  
       perror("Failed on read()\n");
          
       } else 
           
       nickname[ret] = '\0';
  
       if (match(nickname,re))
        {
	    printf("%s is a valid nickname.\n",nickname);
              } 
       
		send(Client_sock, "OK\n", strlen("OK\n"), 0);
      
       

        pthread_mutex_lock(&mutex);
        clients[n]= Client_sock;
        //strcpy(nicknames[n], nickname);
        n++;
        // creating a thread for each client 
        pthread_create(&recvt,NULL,(void *)recvmg,&Client_sock);
        pthread_mutex_unlock(&mutex);
    }
    return 0; 

}