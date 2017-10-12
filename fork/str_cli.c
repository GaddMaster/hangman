/*
 * ------------------------------------------------------
 	Team1: 	Sean Horgan - 		K00196030
		Daniel Gadd - 		
		Samuel McSweeny - 	
 	str_cli.c
 	Description:
		Infinite loop that handles I/O 
		to and from the server until
		the game ends or connection
		terminates.
 * ------------------------------------------------------
 */

 # include "AddedStuff.h"

 void str_cli(FILE *fp, int sockfd)
 {
	int count;
	char i_line[MAXLINE], o_line[MAXLINE]; /* input and output buffer */

	/* -- Main loop to handle I/O until game or connection terminates -- */
	while ((count = read(sockfd, i_line, MAXLINE)) > 0) {
 		printf("%s\n", i_line);		       /* Print input buffer */
 		count = read (0, o_line, MAXLINE);     /* 0 = stdin */
 		write (sockfd, o_line, count);	       /* Write guess to serv */
 	}
} 
