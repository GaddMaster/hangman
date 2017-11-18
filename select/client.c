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
	
	for(int x = 0; x < VALUECOUNT; x++){
		gameStateValues[x] = atoi(string);
		string = strtok(NULL, " ");
		printf("\tGAME STATE VALUE %d\n", gameStateValues[x]);
	}
	
	player.sessionID = gameStateValues[0];

	printf("\tENTER SESSION ID OR -NEW- FOR NEW GAME\n");
	
	char input[30];
	
	fgets(input, sizeof(input), stdin);
	
	printf("\tYOUR INPUT : %s", input);
	
	if(strcmp(input, "NEW"))
	{
		printf("\tNEW GAME SELECTED\n");
		printf("\tPICK DIFFICULTY - 1, 2 OR 3\n");
		fgets(input, sizeof(input), stdin);
		
		printf("\tDIFFICULTY SELECTED : %s\n", input);
		player.gameState = 1;
		player.difficulty = atoi(input);
		
		// CREATE REQUEST FOR WORD STRING
		snprintf(BUFFER, sizeof(BUFFER), "%d 0 %D 0", player.sessionID, player.difficulty);
		printf("PASS\tBUFFER:%s\n", BUFFER);
		
		// SEND SERVER REQUEST FOR WORD
		if( send(network_socket, BUFFER, strlen(BUFFER), 0) != strlen(BUFFER) ) 
		{perror("ERROR\tSEND ERROR\n");
		}else{printf("PASS\tREQUEST FOR WORD SENT\n");}
	}
	else
	{
		//REQUEST TO CONTINUE INITIALISE OLD GAME
		printf("\tTRYING TO RE-INITIALISE OLD GAME\n");
		//PENDING CODE
	}
	
	//	GAME ZONE
	while(player.gameState != 4)
	{
		
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

