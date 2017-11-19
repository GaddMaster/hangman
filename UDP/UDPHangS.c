/*
 * UDP Hangman Server
 * Need to Tidy Up
 * Add ability to choose to play or leave etc..
 */

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
//#include "run_process.h"
#include "AddedStuff.h"

#define PORT 8888

struct sockaddr_in servadder, cliaddr;

int clientlen = sizeof(cliaddr);

extern time_t time ();	/* For seeding RNG */
int maxlives = 12;	/* Stores the maximum number of lives for Player*/
char *word [] = 
{	
     /* Array to store the words from 'words' text file */
     # include "words"
};


int main(void) 
{
	int sockfd;
	int i;
	int bytes;
	char buffer[1024];
	char response [3];

	if ((sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) 
	{
		perror("Socket Creation Has Failed");
	}

	memset((char *) &servadder, 0, sizeof(servadder));

	servadder.sin_family = AF_INET;
	servadder.sin_addr.s_addr = htonl(INADDR_ANY);
	servadder.sin_port = htons(PORT);

	if (bind(sockfd, (struct sockaddr *) &servadder, sizeof(servadder)) == -1) 
	{	
		perror("Binding The Socket Has Failed");
	}

	// 	

	while(1)
	{
		fflush(stdout);

		if((bytes = recvfrom(sockfd, buffer, 1024, 0, (struct sockaddr *) &cliaddr, &clientlen)) == -1)
		{
			perror("recvfrom() Has Failed");
		}
		
		if (sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *) &cliaddr, clientlen) == -1) 
		{
			perror("sendto() Has Failed");
		}

		run_process(sockfd, sockfd);
			
	}

	close(sockfd);

	return 0;
}

 /* -- Contains all the hangman functionality -- */
int run_process(int in, int out)
{

 	int lives = maxlives;			/* Max number of player lives */
 	int game_state = 'I';			/* I = Incomplete */
 	int i, good_guess, word_length;
 	char hostname[MAXLEN];
	char * whole_word, part_word [MAXLEN],
 	guess[MAXLEN], outbuf [MAXLEN];

	/* -- Print to client that they are playing hangman on my machine -- */ 	
	gethostname (hostname, MAXLEN);		
 	sprintf(outbuf, "Playing hangman on host %s:\n", hostname);
 	sendto (out, outbuf, strlen(outbuf), 0, (struct sockaddr *) &cliaddr, sizeof cliaddr);
	

 	/* -- Pick a word at random from the list -- */
	srand(time(NULL));				/* Seed RNG */
 	whole_word = word[rand() % NUM_OF_WORDS];	/* Choose random word */
 	word_length = strlen(whole_word);		/* Get length of word */
 	syslog (LOG_USER | LOG_INFO, "server chose hangman word %s", whole_word);

 	/* -- No letters are guessed Initially -- */
 	for (i = 0; i <word_length; i++)
 		part_word[i]='-';
 	
	part_word[i] = '\0';

	sprintf (outbuf, "WORD: %s LIVES: %d \n", part_word, lives);
 	sendto (out, outbuf, strlen(outbuf), 0, (struct sockaddr *) &cliaddr, sizeof cliaddr);

	/* -- Main loop for guesses and win/lose logic -- */
 	while (game_state == 'I')
 	{
		/* -- Get a letter from player guess -- */
		while (recvfrom(in, guess, MAXLEN, 0, (struct sockaddr *) &cliaddr, &clientlen) <0) {
 			if (errno != EINTR)
 				exit (4);
 			printf ("re-read the startin \n");
 		} /* Re-start read () if interrupted by signal */

 		good_guess = 0;

		/* If guess matches a letter in the word, reveal that letter */
 		for (i = 0; i <word_length; i++) {
 			if (guess [0] == whole_word [i]) {
 				good_guess = 1;
 				part_word [i] = whole_word [i];
 			}
 		}

 		if (! good_guess) lives--;     /* Subtract life for bad guess */

		/* If all letters are guessed correctly */
 		if (strcmp (whole_word, part_word) == 0){
 			game_state = 'W';               /* W ==> User Won */
			sprintf(outbuf, "Congratulations, You Win!\n");
			sendto (out, outbuf, strlen(outbuf), 0, (struct sockaddr *) &cliaddr, sizeof cliaddr);
		}

 		else if (lives == 0) {
 			game_state = 'L';               /* L ==> User Lost */
			sprintf(outbuf, "Hard luck, You Lost!\n");
			write(out,  outbuf, strlen(outbuf));
 			strcpy (part_word, whole_word); /* User Show the word */
 		}

		/* Copy part_word and lives to outgoing buffer */
 		sprintf (outbuf, "WORD: %s LIVES: %d \n", part_word, lives);
 		sendto (out, outbuf, strlen(outbuf), 0, (struct sockaddr *) &cliaddr, sizeof cliaddr);  /* Write to client sock */
 	} // end while
}
