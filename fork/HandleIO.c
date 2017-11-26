/*
 * ------------------------------------------------------
 	Team1: 	Sean Horgan - 		K00196030
		Daniel Gadd - 		K00202350
		Samuel McSweeny - 	K00200955
 	
	Name:	HandleIO.c

 	Description:
		Infinite loop that handles I/O 
		to and from the server until
		the game ends or connection
		terminates.
		
		Uses select() to block until
		a file descriptor is read for
		reading or writing.
	
	Compile:
		Compile: gcc -o cli Client.c HandleIO.c
 * ------------------------------------------------------
 */

 # include "AddedStuff.h"

   void str_cli(FILE *fp, int sockfd)
   {
	int maxfdp1;
	fd_set rset;
	char buf[MAXLINE];
	int n;
/*--------------------------Setup for select()--------------------------------*/
	FD_ZERO(&rset);
	for ( ; ; ) {	   
	   FD_SET(fileno (fp), & rset); 					// fp - bit corresponding to standard I/O file pointer
	   FD_SET(sockfd, &rset); 						// sockfd - bit corresponding to socket
	   
	   maxfdp1 = max(fileno (fp), sockfd) + 1;				/* fileno: convert I/O file pointer to corresponding descriptor
	   									   max: calculate the maximum of the two descriptors */

	   select(maxfdp1, &rset, NULL, NULL, NULL);				// time limit is NULL so it can block until something is ready

/*--------------- Receive data from server, write to console -----------------*/
/* 	
	If the socket is readble upon return from select() the echoed
	line is read and output by write, if read contains 0 bytes
	then that is an eof and the connection will terminate  
*/		
	   if (FD_ISSET(sockfd, &rset)) { 					// Socket is readable	   
	      if((n = read(sockfd, buf, MAXLINE)) == 0)				// data	
  	         return;
	      write(fileno(stdout), buf, n); 					// Write output to console
	   }

/*---------- Read keyboard input, place in buffer, write to server -----------*/
/*
	If the input is readable, copy it to the buffer then write
	it to the server socket.
*/	 
	   if (FD_ISSET(fileno (fp), &rset)) { 					// Input is readable
	       if((n = read(fileno(fp), buf, MAXLINE)) == 0)			// Read keyboard input, store in buffer
		  return;
	       write(sockfd, buf, n);						// Write contents of buffer to server
	   }
       }
   }
