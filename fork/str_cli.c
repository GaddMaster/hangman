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
	int maxfdp1;
	fd_set rset;
	//char sendline[MAXLINE], recvline[MAXLINE];
	char buf[MAXLINE];
	int n;

	// fp - bit corresponding to standard I/O file pointer
	// sockfd - bit corresponding to socket
	FD_ZERO(&rset);
	for ( ; ; ) {	   
	   FD_SET(fileno (fp), & rset);
	   FD_SET(sockfd, &rset);
	   
	   /* fileno: convert I/O file pointer to corresponding descriptor */
	   /* max: calculate the maximum of the two descriptors */
	   maxfdp1 = max(fileno (fp), sockfd) + 1;

	   /* -- Call select() -- */
	   /* time limit is NULL so it can block until something is ready */
	   select(maxfdp1, &rset, NULL, NULL, NULL);

	   /* If the socket is readble upon return from select() the echoed
	      line is read and output by write */
	   if (FD_ISSET(sockfd, &rset)) { /* socket is readable */
	      if((n = read(sockfd, buf, MAXLINE)) == 0)
	            error("str_cli: Server terminated prematurely");
	      //printf("%s", recvline);
	      write(fileno(stdout), buf, n);
	   }

	   /* if the input is readable copy it to the outgoing buffer
	      then write it to the server socket */
	   if (FD_ISSET(fileno (fp), &rset)) { /* input is readable */
	      if((n = read(fileno(fp), buf, MAXLINE)) == 0)
		 return;
	      write(sockfd, buf, n);
	      //system("clear");    /* Uncomment to clear console every guess */
	   }
       }
   }

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

