/*
 * ------------------------------------------------------
 	Team1: 	Sean Horgan - 		K00196030
		Daniel Gadd - 		K00202350
		Samuel McSweeny - 	
 	
	Name:	str_cli.c

 	Description:
		Infinite loop that handles I/O 
		to and from the server until
		the game ends or connection
		terminates.
		
		Uses select() to block until
		a file descriptor is read for
		reading or writing.
	
	Compile:
		gcc -o cli TCPClient.c str_cli.c
 * ------------------------------------------------------
 */

 # include "AddedStuff.h"

   void str_cli(FILE *fp, int sockfd)
   {
	int maxfdp1, stdineof;
	fd_set rset;
	char buf[MAXLINE];
	int n;

	/* fp - bit corresponding to standard I/O file pointer */
	stdineof = 0;
	FD_ZERO(&rset);
	for ( ; ; ) {
	   if(stdineof == 0)	   
		FD_SET(fileno (fp), & rset);
	   FD_SET(sockfd, & rset); /* sockfd - bit corresponding to socket */
	   
	   /* fileno: convert I/O file pointer to corresponding descriptor */
	   /* max: calculate the maximum of the two descriptors */
	   maxfdp1 = max(fileno (fp), sockfd) + 1;

	   /* -- Call select() -- */
	   /* time limit is NULL so it can block until something is ready */
	   select(maxfdp1, &rset, NULL, NULL, NULL);

	   /* If the socket is readble upon return from select() the echoed
	      line is read and output by write, if read contains 0 bytes
	      then that is an eof and the connection will terminate */
	   if (FD_ISSET(sockfd, &rset)) { /* socket is readable */
	      if( (n = read(sockfd, buf, MAXLINE)) == 0)
  	         return;
	      write(fileno(stdout), buf, n); /* Write output to console */
	   }

	   /* If the input is readable copy it to the outgoing buffer
	      then write it to the server socket */
	   if (FD_ISSET(fileno (fp), &rset)) { /* input is readable */
	      if( (n = read(fileno(fp), buf, MAXLINE)) == 0) {
		 continue;
	      }
	      write(sockfd, buf, n);	/* Write contents of buffer to server */
	   }
       }// end for
   }// end str_cli()

/* -- REPLACE ABOVE CODE WITH BELOW CODE FOR NON-SELECT I/O -- */

// void str_cli(FILE *fp, int sockfd)
// {
//	int count;
//	char i_line[MAXLINE], o_line[MAXLINE]; /* input and output buffer */
//
//	/* -- Main loop to handle I/O until game or connection terminates -- */
//	while ((count = read(sockfd, i_line, MAXLINE)) > 0) {
 //		printf("%s\n", i_line);		       /* Print input buffer */
 //		count = read (0, o_line, MAXLINE);     /* 0 = stdin */
 //		write (sockfd, o_line, count);	       /* Write guess to serv */
 //	}
 //} 

