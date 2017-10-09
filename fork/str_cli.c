/*
 * ------------------------------------------------------
 	Team1: 	Sean Horgan - 		K00196030
		Daniel Gadd - 		
		Samuel McSweeny - 	
 	str_cli.c
 	Description:
		Handles I/O to and from the server.
 * ------------------------------------------------------
 */

 # include "include.h"

 void str_cli(FILE *fp, int sockfd)
 {
	char sendline[MAXLINE], recvline[MAXLINE];

	while(fgets(sendline, MAXLINE, fp) != NULL) {
		
		write(sockfd, sendline, strlen (sendline));

		if (read(sockfd, recvline, MAXLINE) == 0)
			error("str_cli: server terminated prematurely");
			
		fputs(recvline, stdout);
	}
} 
