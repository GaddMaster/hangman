
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

char *word [] = {
     # include "../words"
 };
  
#define TRUE   1
#define FALSE  0
#define PORT 8888

struct Players{
	int ID;
	int client_socket;
	char word[30];
	char word_state[30];
	int difficulty;
	int guesses;
	int sessionID;
	int state;	// INACTIVE:1	DIFFICULTY:2	ACTIVE:3 
};
 
int main(int argc , char *argv[])
{
    int opt = TRUE;
    int serv_socket;
    int addrlen;
    int new_socket;
    
    struct Players players[3];
    int max_players = 3;
    int max_guess = 10;
    
    int activity;
    int i;
    int valread;
    int sd;
    int max_sd;
    
    struct sockaddr_in address;
      
    char buffer[1024];
      
    fd_set master_set;
      
    char message[1024];
     char * welcome = "WELCOME TO HANGMAN EXPRESS \r\n";
    char * serverFull = "SERVER IS FULL - PLEASE TRY AGAIN LATER \r\n";
  
    for (i = 0; i < max_players; i++) {players[i].client_socket = 0;players[i].ID == i;} //INITILISE ALL CLIENT SOCKETS TO ZERO
    
 	// PICK RANDOM WORD FROM OUR LIST
	//srand(time(NULL));
 	//whole_word = word[rand() % NUM_OF_WORDS];git pull origin master

 	//word_length = strlen(whole_word);
 	//for (i = 0; i <word_length; i++) part_word[i]='-';
	//part_word[i] = '\0';
      
      
    //CREATE MAIN SOCKET
    if( (serv_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0){
        perror("ERROR\tSOCKET ERROR");
        exit(EXIT_FAILURE);
    }else{printf("PASS\tSOCKET CREATED AT %d\n", serv_socket);}
  
  
    //MAIN SOCKET ALLOW MULTIPLY CONNECTIONS
    if( setsockopt(serv_socket, SOL_SOCKET, SO_REUSEADDR, (char * ) & opt, sizeof(opt)) < 0 ){
        perror("ERROR\tSET SOCKET OPTION ERROR");
        exit(EXIT_FAILURE);
    }else{printf("PASS\tSOCKET MULTIPLY CONNECTIONS SET\n");}
  

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );
      
      
    //bind the socket to localhost port 8888
    if (bind(serv_socket, (struct sockaddr *)&address, sizeof(address))<0){
        perror("ERROR\tBIND ERROR");
        exit(EXIT_FAILURE);
    }else{printf("PASS\tSOCKET BINDED TO ADDRESS\n");}
    
    
    printf("PASS\tLISTENING PORT : %d \n", PORT);
    
     
    //LISTEN - SET MAX 3 CONNECTION - TESTING
    if (listen(serv_socket, 3) < 0){
        perror("ERROR\tLISTENING ERROR");
        exit(EXIT_FAILURE);
    }else{printf("PASS\tSOCKET LISTENING\n");}
      
      
    //WAITING FOR CONNECTIONS MESSAGE
    addrlen = sizeof(address);
    puts("PASS\tSERVER WAITING FOR CONNECTIONS . . . . \n\n");
     
    while(TRUE) 
    {
        FD_ZERO(&master_set); // CLEAR MASTER SET
  
        //ADD MAIN SOCKET TO SET
        FD_SET(serv_socket, &master_set);
        max_sd = serv_socket;
        
        
        /////////////////////////////////////////////////////////////////////////////////
        //ADD CHILD SOCKET TO SET
        for ( i = 0 ; i < max_players ; i++) 
        {
            sd = players[i].client_socket; // SOCKET DESCRIPTOR
            if(sd > 0) FD_SET( sd , &master_set); // ADD TO READ LIST IOF VALID SOCKET DESCRIPTOR
            if(sd > max_sd) max_sd = sd; // WE NEED HIGHEST FILE DESCRIPTOR FOR THE SELECT FUNCTION
        }
        /////////////////////////////////////////////////////////////////////////////////
    
        
        activity = select( max_sd + 1 , &master_set , NULL , NULL , NULL); // CALL SELECT AND WAIT FOR ACTIVITY - NO TIMEOUT SELECTED SO INDEFINITE
    
    
        if ((activity < 0) && (errno != EINTR)) printf("ERROR\tSELECT ERROR - FUCKED\n");
        
          
          
        /////////////////////////////////////////////////////////////////////////////////
        // CHECK MASTER SOCKET FOR INCOMING CONNECTION
        if (FD_ISSET(serv_socket, &master_set)) 
        {
            if ((new_socket = accept(serv_socket, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
            {
                perror("ERROR\tACCEPT ERROR");
                exit(EXIT_FAILURE);
            }else{printf("PASS\tNEW PLAYER FD:%d IP:%s PORT:%d \n" , new_socket , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));}
              
            //SEND WELCOME MESSAGE
            if(send(new_socket, &welcome, strlen(welcome), 0) != strlen(welcome))
            {
            	perror("ERROR\tSEND WELCOME");
            }else{printf("PASS\tSEND WELCOME\n");}
              
            //ADD NEW PLAYER - LOOP PLAYER MAX LIMIT
            for (i = 0; i < max_players; i++) 
            {
                //IF WE FIND A EMPTY PLAYER POSITION
                if( players[i].client_socket == 0 )
                {
                	//printf("PASS\tEMPTY PLAYER SLOT FOUND\n");//DEBUGGING
                	
                    players[i].client_socket = new_socket;
                    
                    players[i].sessionID = rand() % 10000;
                    //int unique = TRUE;
                    
                    //printf("PASS\tINITIAL SESSION ID CREATED : %d\n", players[i].sessionID);//DEBUGGING
                    
                    //CHECK IF SESSION ID IS UNIQUE 
                   	//do
                   	//{
						//for (int checker = 0; checker < max_players; checker++) 
						//{
							//if(players[i].sessionID == players[checker].sessionID && players[i].ID != players[checker].ID) 
							//{
								//unique = FALSE;
								//printf("ERROR\tCOPY FOUND\n");//DEBUGGING
								//printf("ERROR\tPLAYER:%d \t CHECKER:%d\n", players[i].sessionID, players[checker].sessionID);//DEBUGGING
							//}
						//}
						
						//printf("PASS\tCOPY NOT FOUND\n");//DEBUGGING
						
                    //} while(!unique);
				    
                   // printf("PASS\tSESSION ID %d HAS BEEN ESTABLISHED \n", players[i].sessionID);
                    
					players[i].state = 1; // INACTIVE STATE
					
				    // SEND INITIAL MESSAGE - CHECK RETURN IS LENGTH OF STRING
				    
				    snprintf(buffer, sizeof(buffer), "%d %d", players[i].sessionID, players[i].state);
				    
				    printf("PASS\tBUFFER:%s\n", buffer);
				    
				    
				    //if(send(new_socket, buffer, strlen(buffer), 0) != strlen(buffer) ) 
				    //{
				        //perror("ERROR\tSEND ERROR\n");
				    //}else{printf("PASS\tINITIAL MESSAGE SEND TO SOCKET %d\n", new_socket);}
				      
                }
                else
                {
                	printf("PASS\tSERVER FULL - PLAYER REQUEST DENIED %d\n", players[i].client_socket);
                	
				    if( send(new_socket, serverFull, strlen(serverFull), 0) != strlen(serverFull) ) 
				    {
				        perror("ERROR\tSEND ERROR\n");
				    }else{printf("PASS\tSEND SERVER FULL MESSAGE TO SOCKET %d\n", new_socket);}
                		
                }
            }
        }
        /////////////////////////////////////////////////////////////////////////////////
        
        
          
        //IO OPERATION ON ANOTHER SOCKET
        for (i = 0; i < max_players; i++) 
        {
            sd = players[i].client_socket;
              
            if (FD_ISSET( sd , &master_set)) 
            {
                //CHECK FOR CLOSING AND INCOMING MESSAGE
                if ((valread = read( sd , buffer, 1024)) == 0)
                {
                    // CLIENT DISCONNECTED
                    getpeername(sd , (struct sockaddr*)&address , (socklen_t*)&addrlen);
                    printf("PLAYER DISCONNECTED IP:%s PORT:%d \n" , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));
                      
                    //CLOSE SOCKET AND MARK AS ZERO
                    close( sd );
                    players[i].client_socket = 0;
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


static char *rand_string(char *str, size_t size)
{
    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWX0123456789ledobviouso";
    if (size) {
        --size;
        for (size_t n = 0; n < size; n++) {
            int key = rand() % (int) (sizeof charset - 1);
            str[n] = charset[key];
        }
        str[size] = '\0';
    }
    return str;
}























