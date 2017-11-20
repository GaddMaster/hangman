/*
 * UDP Client
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

#define PORT 8888

int main(void) 
{
	struct sockaddr_in cliaddr;
 	int sockfd;
	int i;
	int clientlen=sizeof(cliaddr);
	int bytes;
	char clientguess[1024];
	char partword[1024];
 	char format[1024];

	if ((sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) 
	{
		perror("Socket Creation Has Failed");
	}

	memset((char *) &cliaddr, 0, sizeof(cliaddr));
	cliaddr.sin_family = AF_INET;
	cliaddr.sin_port = htons(PORT);


	while(1)
	{
		fgets(clientguess, 1024, stdin);

		sprintf(format, "%sockfd", clientguess);

		if (sendto(sockfd, format, strlen(clientguess), 0,(struct sockaddr *) &cliaddr, clientlen) == -1)
		{
			 perror("sendto() Failed");
		}

		bytes = recvfrom(sockfd, partword, 1024, 0, (struct sockaddr *) &cliaddr, &clientlen);	 	

		partword[bytes] = '\0'; 									
		printf(partword, strlen(partword), bytes);										

	}

	close(sockfd);
	return 0;
}
