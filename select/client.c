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
#define VALUECOUNT 2
#define LINESIZE 80

struct Player{
	int ID;
	int client_socket;
	char word[30];
	char word_state[30];
	int difficulty;
	int guesses;
	int sessionID;
	int gameState;	// ACTIVE:1	GAME OVER:0
};

void printHangMan(char word[30], char wordState[30], int guesses);

void clear();

int main (int argc, char * argv [])
{
	clear();

	int network_socket;
	//int client_socket;
	
    char BUFFER[1024]; // PROTOCOL - SESSION ID - TYPE - FULL WORD - WORD STATE - GAME STATE
    
    char * string;
    
    char input[5];
    
 	 char i_line[LINESIZE];
 	 char o_line[LINESIZE];
 	 int gameStateValues[VALUECOUNT];
 	 
 	 struct Player player;
      
    //CREATE MAIN SOCKET
    if( (network_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0){
        perror("ERROR\tSOCKET ERROR");
        exit(EXIT_FAILURE);
    }else{printf("PASS\tSOCKET CREATED AT %d\n", network_socket);}
 
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(PORT);
	
    //CONNECT TO SERVER
    if (connect(network_socket, (struct sockaddr *)&server_address, sizeof(server_address))<0){
        perror("ERROR\tBIND ERROR");
        exit(EXIT_FAILURE);
    }else{printf("PASS\tCONNECTED TO NETWORK SOCKET ADDRESS %d\n", network_socket);}

	struct sockaddr_in client_address;
	int size = sizeof(struct sockaddr);
	
	if(recv(network_socket, BUFFER, 1024, 0) < 0)
	{
		printf("ERROR\tRECIEVE ERROR\n");
	}else{printf("PASS\tRESPONSE RECIEVED: %s\n", BUFFER);}

	//GET MY MACNHINE NAME
	int socket_name = getsockname(network_socket, (struct sockaddr * ) &client_address, &size);
	
	string = strtok(BUFFER, " ");
	
	//for(int x = 0; x < VALUECOUNT; x++){
		//gameStateValues[x] = atoi(string);
		//string = strtok(NULL, " ");
		//printf("\tGAME STATE VALUE %d\n", gameStateValues[x]);}
	
	//player.sessionID = gameStateValues[0];
	
	//GET PLAYER SESSION ID OR REQUEST NEW GAME
	printf("\tENTER -SESSION ID- OR -NEW- FOR NEW GAME : \t");
	fgets(input, sizeof(input), stdin);
	printf("%s\n", input);
	
	if(strcmp(input, "NEW") || strcmp(input, "new") || strcmp(input, "n") || strcmp(input, "N"))
	{
	
		printf("\tNEW GAME SELECTED\n");
		
		printf("\tPICK DIFFICULTY : 1 - 3\n");
		fgets(input, sizeof(input), stdin);
		printf("\tDIFFICULTY SELECTED : %s\n", input);
		player.difficulty = atoi(input);

		// CREATE REQUEST FOR WORD STRING
		memset(&BUFFER[0], 0, sizeof(BUFFER));
		BUFFER[0] = '0';
		BUFFER[1] = ' ';
		switch(player.difficulty){
			case 1  : BUFFER[2] = '1';break;
			case 2  : BUFFER[2] = '2';break;
			case 3  : BUFFER[2] = '3';break;
			default : BUFFER[2] = '1';break;
		}
		BUFFER[3] = ' ';
		BUFFER[4] = '0';
		BUFFER[sizeof(BUFFER)] = '\0';
			
		// SEND SERVER REQUEST FOR WORD
		printf("SEND\tSERVER:%s\n", BUFFER);
		if(send(network_socket, BUFFER, strlen(BUFFER), 0) != strlen(BUFFER)) 
		{perror("ERROR\tSEND ERROR\n");
		}else{printf("PASS\tREQUEST FOR WORD SENT : %s\n", BUFFER);}
		
		if(recv(network_socket, BUFFER, 1024, 0) < 0)
		{printf("ERROR\tRECIEVE ERROR\n");
		}else{printf("PASS\tRESPONSE RECIEVED:%s\n", BUFFER);}

	}
	else
	{
		//REQUEST TO CONTINUE INITIALISE OLD GAME
		printf("\tTRYING TO RE-INITIALISE OLD GAME\n");
		//PENDING CODE
		
		// CREATE REQUEST FOR WORD STRING
		snprintf(BUFFER, sizeof(BUFFER), "%d %d", player.sessionID, player.difficulty);
		printf("PASS\tBUFFER:%s\n", BUFFER);
		
		// SEND SERVER REQUEST FOR WORD
		if( send(network_socket, BUFFER, strlen(BUFFER), 0) != strlen(BUFFER) ) 
		{perror("ERROR\tSEND ERROR\n");
		}else{printf("PASS\tREQUEST TO CONTINUE SESSION SENT\n");}
		
	}
	
	//	GAME ZONE
	while(player.gameState != 4 || player.gameState != 5)
	{
		printf("GAME\tPLAYING\n");
		break;
	}
	
	
	
	
	
	sleep(1000);
    
    printf("PASS\tCLOSING NETWORK SOCKET\n");
    
 	close(network_socket);
 	
 	printf("PASS\tNETWORK SOCKET CLOSED\n");
    
    return 0;

}

void clear()
{
	system("@cls||clear");
}

int printHangman(char word[30], char wordState[30], int guesses)
{
	clear(); // CLEAR CONSOLE FOR NEW STATE TO BE DISPLAYED
	
	int gameOver = 0;
	
	printf("\tHANGMAN EXPRESS");
	
	switch(guesses){
		case 0:
			printf("\t--------------------\n");
			printf("\t|               |   \n");
			printf("\t|               O   \n");
			printf("\t|              -I-  \n");
			printf("\t|               |   \n");
			printf("\t|               ^   \n");
			printf("\t|                   \n");
			printf("\t|                   \n");
			printf("\t--------------------\n");
			gameOver = TRUE;
			break;
		case 1:
			printf("\t--------------------\n");
			printf("\t|               |   \n");
			printf("\t|               O   \n");
			printf("\t|              -I-  \n");
			printf("\t|               |   \n");
			printf("\t|                   \n");
			printf("\t|                   \n");
			printf("\t|                   \n");
			printf("\t--------------------\n");
			gameOver = FALSE;
			break;
		case 2:
			printf("\t--------------------\n");
			printf("\t|               |   \n");
			printf("\t|               O   \n");
			printf("\t|              -I-  \n");
			printf("\t|                   \n");
			printf("\t|                   \n");
			printf("\t|                   \n");
			printf("\t|                   \n");
			printf("\t--------------------\n");
			gameOver = FALSE;
			break;
		case 3:
			printf("\t--------------------\n");
			printf("\t|               |   \n");
			printf("\t|               O   \n");
			printf("\t|              -I   \n");
			printf("\t|                   \n");
			printf("\t|                   \n");
			printf("\t|                   \n");
			printf("\t|                   \n");
			printf("\t--------------------\n");
			gameOver = FALSE;
			break;
		case 4:
			printf("\t--------------------\n");
			printf("\t|               |   \n");
			printf("\t|               O   \n");
			printf("\t|               I   \n");
			printf("\t|                   \n");
			printf("\t|                   \n");
			printf("\t|                   \n");
			printf("\t|                   \n");
			printf("\t--------------------\n");
			gameOver = FALSE;
			break;
		case 5:
			printf("\t--------------------\n");
			printf("\t|               |   \n");
			printf("\t|               O   \n");
			printf("\t|                   \n");
			printf("\t|                   \n");
			printf("\t|                   \n");
			printf("\t|                   \n");
			printf("\t|                   \n");
			printf("\t--------------------\n");
			gameOver = FALSE;
			break;
		case 6:
			printf("\t--------------------\n");
			printf("\t|               |   \n");
			printf("\t|                   \n");
			printf("\t|                   \n");
			printf("\t|                   \n");
			printf("\t|                   \n");
			printf("\t|                   \n");
			printf("\t|                   \n");
			printf("\t--------------------\n");
			gameOver = FALSE;
			break;
		case 7:
			printf("\t--------------------\n");
			printf("\t|                   \n");
			printf("\t|                   \n");
			printf("\t|                   \n");
			printf("\t|                   \n");
			printf("\t|                   \n");
			printf("\t|                   \n");
			printf("\t|                   \n");
			printf("\t--------------------\n");
			gameOver = FALSE;
			break;
		default:
			printf("\t--------------------\n");
			printf("\t|               |   \n");
			printf("\t|               O   \n");
			printf("\t|              -I-  \n");
			printf("\t|               |   \n");
			printf("\t|               ^   \n");
			printf("\t|                   \n");
			printf("\t|                   \n");
			printf("\t--------------------\n");
			gameOver = TRUE;
			break;
	}
	
	printf("\t%s", wordState);
	
	printf("\tGUESS LETTER\n\n");
	
	return gameOver;
}

