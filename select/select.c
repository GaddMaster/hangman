
//USE COMMAND ON CONSOLE AS CLIENT - telnet localhost 8888

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
 
int main(int argc , char *argv[])
{
    int opt = TRUE;
    int serv_socket;
    int addrlen;
    int new_socket;
    int client_socket[30];
    int max_clients = 30;
    int activity;
    int i;
    int valread;
    int sd;
    int max_sd;
    
    struct sockaddr_in address;
      
    char buffer[1025];
      
    fd_set master_set;
      
    char * message = "WELCOME TO MY SERVER\r\n";
  
    for (i = 0; i < max_clients; i++) client_socket[i] = 0; //INITILISE ALL CLIENT SOCKETS TO ZERO
      
      
    //CREATE MAIN SOCKET
    if( (serv_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0){
        perror("SOCKET ERROR");
        exit(EXIT_FAILURE);
    }else{printf("SOCKET CREATED AT %d\n", serv_socket);}
  
  
    //MAIN SOCKET ALLOW MULTIPLY CONNECTIONS
    if( setsockopt(serv_socket, SOL_SOCKET, SO_REUSEADDR, (char * ) & opt, sizeof(opt)) < 0 ){
        perror("SET SOCKET OPTION ERROR");
        exit(EXIT_FAILURE);
    }else{printf("SOCKET MULTIPLY CONNECTIONS SET\n");}
  

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );
      
      
    //bind the socket to localhost port 8888
    if (bind(serv_socket, (struct sockaddr *)&address, sizeof(address))<0){
        perror("BIND ERROR");
        exit(EXIT_FAILURE);
    }else{printf("SOCKET BINDED TO ADDRESS\n");}
    
    
    printf("LISTENING PORT : %d \n", PORT);
    
     
    //LISTEN - SET MAX 3 CONNECTION - TESTING
    if (listen(serv_socket, 3) < 0){
        perror("LISTENING ERROR");
        exit(EXIT_FAILURE);
    }else{printf("SOCKET LISTENING\n");}
      
      
    //WAITING FOR CONNECTIONS MESSAGE
    addrlen = sizeof(address);
    puts("SERVER WAITING FOR CONNECTIONS . . . . ");
     
    while(TRUE) 
    {
        FD_ZERO(&master_set); // CLEAR MASTER SET
  
        //ADD MAIN SOCKET TO SET
        FD_SET(serv_socket, &master_set);
        max_sd = serv_socket;
        
        
        /////////////////////////////////////////////////////////////////////////////////
        //ADD CHILD SOCKET TO SET
        for ( i = 0 ; i < max_clients ; i++) 
        {
            sd = client_socket[i]; // SOCKET DESCRIPTOR
            if(sd > 0) FD_SET( sd , &master_set); // ADD TO READ LIST IOF VALID SOCKET DESCRIPTOR
            if(sd > max_sd) max_sd = sd; // WE NEED HIGHEST FILE DESCRIPTOR FOR THE SECLECT FUNCTION
        }
        /////////////////////////////////////////////////////////////////////////////////
    
        
        activity = select( max_sd + 1 , &master_set , NULL , NULL , NULL); // CALL SELECT AND WAIT FOR ACTIVITY - NO TIMEOUT SELECTED SO INDEFINITE
    
    
        if ((activity < 0) && (errno != EINTR)) printf("SELECT ERROR - FUCKED");
        
          
          
        /////////////////////////////////////////////////////////////////////////////////
        // CHECK MASTER SOCKET FOR INCOMING CONNECTION
        if (FD_ISSET(serv_socket, &master_set)) 
        {
            if ((new_socket = accept(serv_socket, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
            {
                perror("ACCEPT ERROR");
                exit(EXIT_FAILURE);
            }else{printf("NEW CLIENT ACCEPTED : %d", new_socket);}
          
            //INFORM USER OF SOCKET NUMBER - USED IN SEND AND RECIEVE COMMAND
            printf("NEW CONNECTION ON SOCKET FILE DESCRIPTOR %d \t IP %s \t PORT %d \n" , new_socket , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));
        
            // SEND WELCOME MESSAGE
            if( send(new_socket, message, strlen(message), 0) != strlen(message) ) 
            {
                perror("SEND ERROR");
            }else{printf("SEND WELCOME MESSAGE TO SOCKET %d", new_socket);}
              
            printf("WELCOME MESSAGE SENT\n");
              
            //add new socket to array of sockets
            for (i = 0; i < max_clients; i++) 
            {
                //if position is empty
                if( client_socket[i] == 0 )
                {
                    client_socket[i] = new_socket;
                    printf("SOCKET ADDED AS %d\n" , i);
                     
                    break;
                }
            }
        }
        /////////////////////////////////////////////////////////////////////////////////
        
        
          
        //IO OPERATION ON ANOTHER SOCKET
        for (i = 0; i < max_clients; i++) 
        {
            sd = client_socket[i];
              
            if (FD_ISSET( sd , &master_set)) 
            {
                //CHECK FOR CLOSING AND INCOMING MESSAGE
                if ((valread = read( sd , buffer, 1024)) == 0)
                {
                    // CLIENT DISCONNECTED
                    getpeername(sd , (struct sockaddr*)&address , (socklen_t*)&addrlen);
                    printf("Host disconnected , ip %s , port %d \n" , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));
                      
                    //CLOSE SOCKET AND MARK AS ZERO
                    close( sd );
                    client_socket[i] = 0;
                }
                else//ECHO MESSAGE TO CLIENT
                {
                    //SET STRING TERMINATING BYTE AT END
                    buffer[valread] = '\0';
                    send(sd , buffer , strlen(buffer) , 0 );
                }
            }
        }
    }
      
    return 0;
} 