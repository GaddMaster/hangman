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
	int maxfdp1;
	fd_set rset;
	char buf[MAXLINE];
	int n;

<<<<<<< HEAD
	/* fp - bit corresponding to standard I/O file pointer */
	stdineof = 0;
	FD_ZERO(&rset);
	for ( ; ; ) {
	   if(stdineof == 0)	   
		FD_SET(fileno (fp), & rset);
	   FD_SET(sockfd, & rset); /* sockfd - bit corresponding to socket */
=======
	// fp - bit corresponding to standard I/O file pointer
	// sockfd - bit corresponding to socket
	FD_ZERO(&rset);
	for ( ; ; ) {	   
	   FD_SET(fileno (fp), & rset);
	   FD_SET(sockfd, &rset);
>>>>>>> origin
	   
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
<<<<<<< HEAD
	      if( (n = read(sockfd, buf, MAXLINE)) == 0)
  	         return;
	      write(fileno(stdout), buf, n); /* Write output to console */
=======
	      if((n = read(sockfd, buf, MAXLINE)) == 0)
	            error("str_cli: Server terminated prematurely");
	      //printf("%s", recvline);
	      write(fileno(stdout), buf, n);
>>>>>>> origin
	   }

	   /* If the input is readable copy it to the outgoing buffer
	      then write it to the server socket */
	   if (FD_ISSET(fileno (fp), &rset)) { /* input is readable */
<<<<<<< HEAD
	      if( (n = read(fileno(fp), buf, MAXLINE)) == 0) {
		 continue;
	      }
	      write(sockfd, buf, n);	/* Write contents of buffer to server */
=======
	      if((n = read(fileno(fp), buf, MAXLINE)) == 0)
		 return;
	      write(sockfd, buf, n);
	      //system("clear");    /* Uncomment to clear console every guess */
>>>>>>> origin
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

