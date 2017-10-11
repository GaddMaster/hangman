/*
 * ------------------------------------------------------
 	Team1: 	Sean Horgan - 		K00196030
		Daniel Gadd - 		
		Samuel McSweeny - 	
 	AddedStuff.h
 	Description:
		Include this file into all .c files
		to include the necessary headers and
		function definitions.
 * ------------------------------------------------------
 */
 #include <stdio.h>
 #include <stdlib.h>
 #include <unistd.h>
 #include <errno.h>
 #include <string.h>
 #include <sys/types.h>
 #include <sys/socket.h>
 #include <netinet/in.h>
 #include <netdb.h>
 #include <arpa/inet.h>
 #include <sys/wait.h>
 #include <signal.h>
 #include <syslog.h>
 #include <time.h>

 # define NUM_OF_WORDS (sizeof (word) / sizeof (word [0]))
 # define MAXLEN 80 		/* Maximum size in the world of Any string */
 # define SERV_PORT 9877	/* TCP and UDP client-servers */
 # define SERV_PORT_STR "9877"
 # define BACKLOG 10		/* 2nd argument to listen() */
 # define MAXLINE 4096		/* max text line length */
 # define SA struct sockaddr 	/* Shorten the type cast of pointer argument */

 int run_process(int, int);	/* definition for hangman function */

 void error (char *msg);	/* definitoin for perror wrapper */

 void sigchld_handler(int s);	/* defiintion for child signal handler */

 void *get_in_addr(struct sockaddr *sa);  /* definition for get_in_addr */

 void str_cli(FILE *fp, int sockfd);	  /* definition for cli I/O handler */
