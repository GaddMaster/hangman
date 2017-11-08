#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>

#define TRUE   1
#define FALSE  0
#define PORT 8888
#define IP 127.0.0.1

int main (int argc, char * argv [])
{
	int network_socket;
	int client_socket;
	
    char buffer[1024];
      
    //CREATE MAIN SOCKET
    if( (network_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0){
        perror("SOCKET ERROR");
        exit(EXIT_FAILURE);
    }else{printf("SOCKET CREATED AT %d\n", network_socket);}
 
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(PORT);
	
    //CONNECT TO SERVER
    if (connect(network_socket, (struct sockaddr *)&server_address, sizeof(server_address))<0){
        perror("BIND ERROR");
        exit(EXIT_FAILURE);
    }else{printf("SOCKET BINDED TO ADDRESS\n");}

	struct sockaddr_in client_address;
	int size = sizeof(struct sockaddr);
	
	if(recv(net_sock, buffer, sizeof(buffer), 0))
	{
		
	}

	//GET MY MACNHINE NAME
	int socket_name = getsockname(network_socket, (struct sockaddr * ) &client_address, &size);
    
    sleep(10000);
    
    return 0;
    
    
	
	
}

