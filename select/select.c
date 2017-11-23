
//USE COMMAND ON CONSOLE AS CLIENT - telnet localhost 8888

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>

char * word [] = {
     # include "../words"
 };
  
#define TRUE   1
#define FALSE  0
#define PORT 8888
#define NUMWORDS 9
#define VALUECOUNT 2
#define PLAYERMAX 30

struct Player{
	int ID;
	int client_socket;
	char * word;
	int wordLength;
	char word_state[30];
	int difficulty;
	int guesses;
	int sessionID;
	int state;	// GAME OVER:0	ACTIVE:1 
};

void clear();
 
int main(int argc , char *argv[])
{
	clear();

    int opt = TRUE;
    int serv_socket;
    int addrlen;
    int new_socket;
    
    struct Player players[5];
    int max_players = 5;
    
    int activity;
    int i;
    int x;
    int valread;
    int sd;
    int max_sd;
    
    struct sockaddr_in address;
      
    char buffer[1024];
    
    char * sBUFFER;
    
    char gameStateValues[2];
      
    fd_set master_set;
      
    char message[1024];
     char * welcome = "WELCOME TO HANGMAN EXPRESS \r\n";
    char * serverFull = "SERVER IS FULL - PLEASE TRY AGAIN LATER \r\n";
  
    for (i = 0; i < max_players; i++) {players[i].client_socket = 0;players[i].ID == i;} //INITILISE ALL CLIENT SOCKETS TO ZERO


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
            {perror("accept");exit(EXIT_FAILURE);
            }else{printf("\n\nPASS\tNEW PLAYER FD:%d IP:%s PORT:%d \n" , new_socket , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));}

            // SEND WELCOME MESSAGE
            if( send(new_socket, welcome, strlen(welcome), 0) != strlen(welcome) ) 
            {perror("send");
            }else{printf("PASS\tWELCOME MESSAGE SENT\n");}
		
			
            //ADD NEW PLAYER TO LIST OF PLAYERS
            for (i = 0; i < max_players; i++) 
            {
                //FIND EMTPY POSITION
                if(  players[i].client_socket == 0 )
                {
                    players[i].client_socket = new_socket;
                    printf("PASS\tADDED PLAYER %d TO PLAYER LIST POSITION%d\n" , new_socket, i);
                     
                    break;
                }
            }
        }
        /////////////////////////////////////////////////////////////////////////////////
        
        
        for (i = 0; i < max_players; i++) 
        {
             sd = players[i].client_socket;
              
            if (FD_ISSET( sd , &master_set)) 
            {
            	memset(&buffer[0], 0, sizeof(buffer));
                //CHECK FOR CLOSING AND INCOMING MESSAGE
                if ((valread = read( sd , buffer, 1024)) == 0)
                {
                    // CLIENT DISCONNECTED
                    getpeername(sd , (struct sockaddr*)&address , (socklen_t*)&addrlen);
                    printf("PASS\tPLAYER %d HAS LEFT SERVER IP:%s\tPORT:%d\n" , players[i].ID, inet_ntoa(address.sin_addr) , ntohs(address.sin_port));
                      
                    //CLOSE SOCKET AND MARK AS ZERO
                    close( sd );
                    players[i].client_socket = 0;
                }
                else
                {
                	// PROTOCOL RECEIVE : SESSION ID 0 - 10000 / NEW GAME 0 - DIFFICULTY - GUESS 
                	// PROTOCOL SEND    : WORD STATE - GUESSES 

                	
			printf("TEST\tBUFFER_START:%s\n", buffer);
			
			char * array[2];
			
			int i = 0;
			
			array[i] = strtok(buffer, " ");
			
			while(array[i] != NULL){
				array[++i] = strtok(NULL, " ");
			}
			buffer[sizeof(buffer)] = '\0';
			
			if(array[0] == 0){ // NEW GAME REQUEST
				players[i].word = word[rand() % NUMWORDS];
				for(x = 0; x < sizeof(players[i].word); x++) players[i].word_state[x] = '-';
			}
			else{ //SEARCH SESSION ID AND RE-ACTIVATE OLD GAME
				int found = FALSE;
				for(x = 0; x < PLAYERMAX; x++) if(atoi(array[0]) == players[x].sessionID) found = TRUE;
				
				if(found){
					// PREPARE PLAYER GUESS REQUEST
					memset(&buffer[0], 0, sizeof(buffer));
					snprintf(buffer, sizeof(buffer), "%s %d\n", players[i].word, guesses);
					printf("PASS\tBUFFER_END:%s\n", buffer);
				 	buffer[sizeof(buffer)-2] = '\0';
					send(sd , buffer , strlen(buffer) , 0 );
				}
				else{
					// FAKE SESSION ID - KILL
					memset(&buffer[0], 0, sizeof(buffer));
					snprintf(buffer, sizeof(buffer), "%s", "ERROR\tNOT VALID SESSION ID - ENDING CONNECTION - BYE");
					printf("PASS\tBUFFER_END:%s\n", buffer);
				 	buffer[sizeof(buffer)] = '\0';
					send(sd , buffer , strlen(buffer) , 0 );
				}
				
			}
                }
            }
        }
    }
      
    return 0;
} 


static char * rand_string(char *str, size_t size)
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


void clear()
{
	system("@cls||clear");
}

                







