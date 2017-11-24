
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
#define BUFFSIZE 1023

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
char * prepareString(char * x, char * y);
 
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
    char terminator = '\0';
    
    struct sockaddr_in address;
      
    char BUFFER[1024];
    
    char * pBUFFER;
    
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
                    printf("PASS\tADDED CLIENT %d TO PLAYER LIST POSITION %d\n" , new_socket, i);
                     
                    break;
                }
                else{
			memset(&BUFFER[0], 0, sizeof(BUFFER));
			send(sd , BUFFER , strlen(BUFFER) , 0 );
                }
            }
        }
        /////////////////////////////////////////////////////////////////////////////////
        
        
        for (i = 0; i < max_players; i++) 
        {
             sd = players[i].client_socket;
              
            if (FD_ISSET( sd , &master_set)) 
            {
            	memset(&BUFFER[0], 0, sizeof(BUFFER));
                //CHECK FOR CLOSING AND INCOMING MESSAGE
                if ((valread = read( sd , BUFFER, 1024)) == 0)
                {
			// CLIENT DISCONNECTED
			getpeername(sd , (struct sockaddr*)&address , (socklen_t*)&addrlen);
			printf("PASS\tCLIENT %d HAS LEFT SERVER IP:%s\tPORT:%d\n" , players[i].ID, inet_ntoa(address.sin_addr) , ntohs(address.sin_port));

			//CLOSE SOCKET AND MARK AS ZERO
			close( sd );
			players[i].client_socket = 0;
                }
                else
                {
                	// PROTOCOL RECEIVE : SESSION ID 0 - 10000 / NEW GAME 0 - DIFFICULTY - GUESS 
                	// PROTOCOL SEND    : WORD STATE - GUESSES 

                	
			printf("TEST\tSERVER START REQUEST : %s\n", BUFFER);
			
			char * array[2];
			
			int i = 0;
			
			array[i] = strtok(BUFFER, " ");
			
			while(array[i] != NULL){
				array[++i] = strtok(NULL, " ");
			}
			BUFFER[sizeof(BUFFER)] = '\0';
			
			printf("TEST\tARRAY 0 : %s\n", array[0]);
			printf("TEST\tARRAY 1 : %s\n", array[1]);
			printf("TEST\tARRAY 2 : %s\n", array[2]);
			
			if(strcmp(array[0], "0") == 0){ // NEW GAME REQUEST
			
				printf("NEW\tPLAYER REQUEST\n");

				players[i].word = word[rand() % NUMWORDS];
				
				memset(&players[i].word_state, 0 , sizeof(players[i].word_state));
				
				for(x = 0; x < strlen(players[i].word); x++){
					players[i].word_state[x] = '-';
				}
				players[i].word_state[x] = '|';
				
				printf("TEST\tWORDI:%s\n", players[i].word);
				printf("TEST\tWORDI:%s\n", players[i].word_state);
				
				printf("TEST\tSIZE:%zd\n", strlen(players[i].word_state));
				printf("TEST\tSIZE:%zd\n", strlen(players[i].word));
				
				printf("WORD\tNEW WORD GENERATED - %s\n", players[i].word);
				
				if(strcmp(array[1], "1")){
					players[i].guesses = 3;
					players[i].difficulty = 1;
				}
				else if(strcmp(array[1], "2")){
					players[i].guesses = 4;
					players[i].difficulty = 2;
				}
				else if(strcmp(array[1], "3")){
					players[i].guesses = 5;
					players[i].difficulty = 3;
				}
				else {
					players[i].guesses = 3;
					players[i].difficulty = 1;
				}
				
				int len = strlen(players[i].word)+3;
				
				memset(&BUFFER[0], 0, sizeof(BUFFER));
				printf("TEST\tBUFFER_NOW:%s\n", BUFFER);
				snprintf(BUFFER, len, "%s %d", players[i].word, players[i].guesses);
				printf("PASS\tBUFFER_END:%s\n", BUFFER);
			 	
				send(sd , BUFFER , strlen(BUFFER) , 0 );
			}
			else{ //SEARCH SESSION ID AND RE-ACTIVATE OLD GAME
			
				printf("CONT\tPLAYER REQUEST\n");
			
				int found = FALSE;
				for(x = 0; x < PLAYERMAX; x++) if(atoi(array[0]) == players[x].sessionID) found = TRUE;
				
				if(found){
					int len = strlen(players[i].word)+3;
					memset(&BUFFER[0], 0, sizeof(BUFFER));
					printf("TEST\tBUFFER_NOW:%s\n", BUFFER);
					snprintf(BUFFER, len, "%s %d", players[i].word_state, players[i].guesses);
					printf("PASS\tBUFFER_END:%s\n", BUFFER);
					send(sd , BUFFER , strlen(BUFFER) , 0 );
				}
				else{
					memset(&BUFFER[0], 0, sizeof(BUFFER));
					send(sd , BUFFER , strlen(BUFFER) , 0 );
				}
				
			}
                }
            }
        }
    }
      
    return 0;
} 


void clear(){system("@cls||clear");} // CLEAR TERMINAL OR CMD


                







