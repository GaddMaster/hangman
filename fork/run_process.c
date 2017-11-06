/*
 * ------------------------------------------------------
 	Team1: 	Sean Horgan - 		K00196030
		Daniel Gadd - 		
		Samuel McSweeny - 	
 	run_process.c
 	Description:
		Container for Hangman code. Called by
		the server after fork(). Handles I/O
		to and from the client.
 * ------------------------------------------------------
 */

 # include "AddedStuff.h"

 extern time_t time ();	/* For seeding RNG */

 int maxlives = 12;	/* Stores the maximum number of lives for Player*/
 
 char *word [] = {	/* Array to store the words from 'words' text file */
     # include "../words"
 };

 /* -- Contains all the hangman functionality -- */
 int run_process(int in, int out) {

 	int lives = maxlives;			/* Max number of player lives */
 	int game_state = 'I';			/* I = Incomplete */
 	int i, good_guess, word_length;
 	char hostname[MAXLEN];
	char * whole_word, part_word [MAXLEN],
 	guess[MAXLEN], outbuf [MAXLEN];

	/* -- Print to client that they are playing hangman on my machine -- */ 	
	gethostname (hostname, MAXLEN);		/* i.e. sean-Virtualbox */
 	sprintf(outbuf, "Playing hangman on host %s:\n", hostname);
 	write(out, outbuf, strlen(outbuf));
	

 	/* -- Pick a word at random from the list -- */
	srand(time(NULL));				/* Seed RNG */
 	whole_word = word[rand() % NUM_OF_WORDS];	/* Choose random word */
 	word_length = strlen(whole_word);		/* Get length of word */
 	syslog (LOG_USER | LOG_INFO, "server chose hangman word %s", whole_word);

 	/* -- No letters are guessed Initially -- */
 	for (i = 0; i <word_length; i++)
 		part_word[i]='-';
 	
	part_word[i] = '\0';

	sprintf (outbuf, "%s %d \n", part_word, lives);
 	write (out, outbuf, strlen(outbuf));

	/* -- Main loop for guesses and win/lose logic -- */
 	while (game_state == 'I')
 	{
		/* -- Get a letter from player guess -- */
		while (read (in, guess, MAXLEN) <0) {
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
			write(out,  outbuf, strlen(outbuf));
		}

 		else if (lives == 0) {
 			game_state = 'L';               /* L ==> User Lost */
			sprintf(outbuf, "Hard luck, You Lost!\n");
			write(out,  outbuf, strlen(outbuf));
 			strcpy (part_word, whole_word); /* User Show the word */
 		}

		/* Copy part_word and lives to outgoing buffer */
 		sprintf (outbuf, "%s %d \n", part_word, lives);
 		write (out, outbuf, strlen(outbuf));  /* Write to client sock */
 	} // end while
 }
