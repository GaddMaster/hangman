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
#define BUFFERMAX 1024

struct Player{
	int ID;
	int client_socket;
	char word[30];
	int difficulty;
	int guesses;
	char sessionID[42];
	int gameState;	// ACTIVE:0	GAME OVER:1	WIN:2
};

int printHangman(char word[30], int guesses);

void clear();

int main (int argc, char * argv [])
{
	clear();

	int 	network_socket;
	char 	BUFFER[1024];
	char 	input[5];
	char * 	token;
	int 	len;
	char * 	array[2];
	int 	i;
	char 	place[6];
	 
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
	
	if(recv(network_socket, BUFFER, BUFFERMAX, 0) < 0)
	{printf("ERROR\tRECIEVE ERROR\n");
	}else{printf("PASS\tRESPONSE RECIEVED: %s\n", BUFFER);}

	//GET MY MACNHINE NAME
	int socket_name = getsockname(network_socket, (struct sockaddr * ) &client_address, &size);
	
	token = strtok(BUFFER, " ");
	
	//GET PLAYER SESSION ID OR REQUEST NEW GAME
	printf("\tENTER -SESSION ID- OR -NEW- FOR NEW GAME : \t");
	fgets(input, sizeof(input), stdin);
	printf("%s\n", input);
	
	if(strcmp(input, "NEW") || strcmp(input, "new") || strcmp(input, "n") || strcmp(input, "N"))
	{
	
		printf("\tNEW GAME SELECTED\n");
		
		//printf("\tPICK DIFFICULTY : 1 - 3\n");
		fgets(input, sizeof(input), stdin);
		//printf("\tDIFFICULTY SELECTED : %s\n", input);
		player.difficulty = atoi(input);

		//CREATE REQUEST FOR WORD STRING - MANUAL EXAMPLE OF HANDLE CHARACTER ARRAYS
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
		BUFFER[4] = '0'; // NO CHARACTER GUESS YET
		BUFFER[sizeof(BUFFER)] = '\0'; // ADD TERMINATION CHARACTER
			
		// SEND SERVER REQUEST FOR WORD
		printf("SEND\tSERVER:%s\n", BUFFER);
		if(send(network_socket, BUFFER, strlen(BUFFER), 0) != strlen(BUFFER)) 
		{perror("ERROR\tSEND ERROR\n");
		}else{printf("PASS\tREQUEST FOR WORD SENT : %s\n", BUFFER);}
		
		if(recv(network_socket, BUFFER, 1024, 0) < 0)
		{printf("ERROR\tRECIEVE ERROR\n");
		}else{printf("PASS\tRESPONSE RECIEVED:%s\n", BUFFER);}
		
		player.gameState = 1;
		
		i = 0;
		
		array[i] = strtok(BUFFER, " ");
		
		while(array[i] != NULL){
			array[++i] = strtok(NULL, " ");
		}
		BUFFER[sizeof(BUFFER)] = '\0';
		
		//printf("TEST\tARRAY 0 : %s\n", array[0]);
		//printf("TEST\tARRAY 1 : %s\n", array[1]);
		//printf("TEST\tARRAY 2 : %s\n", array[2]);
		
		strcpy(player.sessionID , array[0]);
		strcpy(player.word , array[1]);
		player.guesses = atoi(array[2]);
		
		//printf("STRUCT\tSESSIONID:%s\n", player.sessionID);
		//printf("STRUCT\tWORD:%s\n", player.word);
		//printf("STRUCT\tGUESSES:%d\n", player.guesses);
	}
	else
	{
		//REQUEST TO CONTINUE INITIALISE OLD GAME
		printf("\tTRYING TO RE-INITIALISE OLD GAME\n");
		sprintf(place, "%s", player.sessionID);
		
		len = strlen(place)+5;
		
		memset(&BUFFER[0], 0, sizeof(BUFFER));
		//printf("TEST\tBUFFER_NOW:%s\n", BUFFER);
		snprintf(BUFFER, len, "%s %d %d", player.sessionID, player.difficulty, 0);
		//printf("PASS\tBUFFER_END:%s\n", BUFFER);
		
		// SEND SERVER REQUEST FOR WORD
		if(send(network_socket, BUFFER, strlen(BUFFER), 0) != strlen(BUFFER) ) 
		{perror("ERROR\tSEND ERROR\n");
		}else{printf("PASS\tREQUEST TO CONTINUE SESSION SENT\n");}
		
		if(recv(network_socket, BUFFER, 1024, 0) < 0)
		{printf("ERROR\tRECIEVE ERROR\n");
		}else{printf("PASS\tRESPONSE RECIEVED: %s\n", BUFFER);}
		
		player.gameState = 1;
		
		i = 0;
		
		array[i] = strtok(BUFFER, " ");
		
		while(array[i] != NULL){
			array[++i] = strtok(NULL, " ");
		}
		BUFFER[sizeof(BUFFER)] = '\0';
		
		//printf("TEST\tARRAY 0 : %s\n", array[0]);
		//printf("TEST\tARRAY 1 : %s\n", array[1]);
		//printf("TEST\tARRAY 2 : %s\n", array[2]);
		
		strcpy(player.sessionID , array[0]);
		strcpy(player.word , array[1]);
		player.guesses = atoi(array[2]);
		
		//printf("STRUCT\tSESSIONID:%s\n", player.sessionID);
		//printf("STRUCT\tWORD:%s\n", player.word);
		//printf("STRUCT\tGUESSES:%d\n", player.guesses);
	}

	
	//GAME ZONE
	while(player.gameState != 1 || player.gameState != 2)
	{
		player.gameState = printHangman(player.word, player.guesses);
		
		if(player.gameState == 1 || player.gameState == 2) break;
		
		//GUESS CHARACTER
		printf("GUESS\tENTER: ");
		fgets(input, sizeof(input), stdin);
		printf("INPUT\t%s\n", input);
		
		len = strlen(player.sessionID) + 5;
		
		////printf("TEST\tLENGTH:%d\n", len);
		//printf("TEST\tSESSION ID:%s\n", player.sessionID);
		//printf("TEST\tDIFFICULTY:%d\n", player.difficulty);
		//printf("TEST\tSESSION ID:%s\n", input);
	
		memset(&BUFFER[0], 0, sizeof(BUFFER));
		printf("TEST\tBUFFER_NOW:%s\n", BUFFER);
		snprintf(BUFFER, len, "%s %d %s", player.sessionID, player.difficulty, input);
		printf("PASS\tBUFFER_END:%s\n", BUFFER);

		//SEND SERVER GUESS
		if(send(network_socket, BUFFER, strlen(BUFFER), 0) != strlen(BUFFER) ) 
		{perror("ERROR\tSEND ERROR\n");
		}else{printf("PASS\tREQUEST TO CONTINUE SESSION SENT\n");}
		
		memset(&BUFFER[0], 0, sizeof(BUFFER));
		if(recv(network_socket, BUFFER, 1024, 0) < 0)
		{printf("ERROR\tRECIEVE ERROR\n");
		}else{printf("PASS\tRESPONSE RECIEVED:%s\n", BUFFER);}
		
		i = 0;
		
		array[i] = strtok(BUFFER, " ");
		
		while(array[i] != NULL){
			array[++i] = strtok(NULL, " ");
		}
		BUFFER[sizeof(BUFFER)] = '\0';
		
		//printf("TEST\tARRAY 0 : %s\n", array[0]);
		////printf("TEST\tARRAY 1 : %s\n", array[1]);
		printf("TEST\tARRAY 2 : %s\n", array[2]);
		
		strcpy(player.sessionID , array[0]);
		strcpy(player.word , array[1]);
		player.guesses = atoi(array[2]);
		
		//printf("STRUCT\tSESSIONID:%s\n", player.sessionID);
		//printf("STRUCT\tWORD:%s\n", player.word);
		////printf("STRUCT\tGUESSES:%d\n", player.guesses);
	
	}
	
	switch(player.gameState){
		case 1:printf("END\tGAME OVER LOSER\n");break;
		case 2:printf("END\tWELL DONE WINNER\n");break;
		default:printf("OPS\tSOMETHING WENT WRONG\n");break;
	}
	
	//REQUEST END GAME AND CLOSE
	memset(&BUFFER[0], 0, sizeof(BUFFER));
	if(send(network_socket, BUFFER, strlen(BUFFER), 0) != strlen(BUFFER) ) 
	{perror("ERROR\tSEND ERROR\n");
	}else{printf("PASS\tREQUEST TO END GAME AND SESSION\n");}
    
    	printf("PASS\tCLOSING NETWORK SOCKET\n");
    
 	close(network_socket);
 	
 	printf("PASS\tNETWORK SOCKET CLOSED\n");
    
    	return 0;

}

void clear()
{
	system("@cls||clear");
}

int printHangman(char word[30], int guesses)
{
	clear(); // CLEAR CONSOLE FOR NEW STATE TO BE DISPLAYED
	
	int gameOver = 0;
	
	printf("\tHANGMAN EXPRESS\n\n");
	
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
	}
	
	printf("\nPLAY\tWORD:%s\tGUESSES:%d\n", word, guesses);
	
	int winner = 1;

	
	for(int x = 0; x < strlen(word); x++) 
		if(word[x] == '-') 
			winner = 0;

	if(winner) return 2;
	else return gameOver;
}



