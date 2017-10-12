
#include	"unp.h"

int main(int argc, char **argv)
{
	int			i;
	int			max_i;
	int 		max_file_descriptor;
	int 		listen_file_descriptor;
	int 		conn_file_descriptor;
	int 		sock_file_descriptor;
	int			nready;
	int			client[FD_SETSIZE];
	
	ssize_t		n;
	ssize_t 	fd_set;			
	ssize_t 	rset;
	ssize_t 	allset;
	
	char 		buf[MAXLINE];
	
	socklen_t			clilen;
	
	struct sockaddr_in	cliaddr;
	struct sockaddr_in	servaddr;
	
	listenfd = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port        = htons(SERV_PORT);

	Bind(listenfd, (SA *) &servaddr, sizeof(servaddr));

	Listen(listenfd, LISTENQ);

	maxfd = listenfd;			// INITIALISE
	maxi = -1;					// INDEX INTO CLIENT[] ARRAY
	
	for (i = 0; i < FD_SETSIZE; i++) { client[i] = -1; } // INDICATE AVAILABLE ENTRY
	
	FD_ZERO(&allset);
	
	FD_SET(listenfd, &allset);


	/////////////////////////////////////////////////////////////////////////////////////
	for ( ; ; ) 
	{
	
		rset = allset;			// STRUCTURE ASSIGNMENT
		
		nready = Select(maxfd + 1, &rset, NULL, NULL, NULL);




		///////////////////////////////////////////////////////
		if (FD_ISSET(listenfd, &rset)) // NEW CLIENT CONNECTION
		{
			clilen = sizeof(cliaddr);
			connfd = Accept(listenfd, (SA *) &cliaddr, &clilen);
			
			#ifdef	NOTDEF
			printf("new client: %s, port %d\n",
					Inet_ntop(AF_INET, &cliaddr.sin_addr, 4, NULL),
					ntohs(cliaddr.sin_port));
			#endif

			for (i = 0; i < FD_SETSIZE; i++)
				if (client[i] < 0) {
					client[i] = connfd;	// SAVE DESCRIPTOR
					break;
				}
			if (i == FD_SETSIZE) { err_quit("TOO MANY CLIENTS"); }

			FD_SET(connfd, &allset);	// ADD NEW DESCRIPTOR TO SET
			if (connfd > maxfd)
				maxfd = connfd;			// FOR SELECT
			if (i > maxi)
				maxi = i;				// MAX INDEX IN CLIENT[] ARRAY

			if (--nready <= 0)
				continue;				// NO MORE READABLE DESCRIPTOR
		}
		////////////////////////////////////////////////////////
		



		////////////////////////////////////////////////////////
		for (i = 0; i <= maxi; i++) 	// CHECK ALL CLIENT FOR DATA
		{
			if ( (sockfd = client[i]) < 0) { continue; }
				
			if (FD_ISSET(sockfd, &rset)) 
			{
				if ( (n = Read(sockfd, buf, MAXLINE)) == 0) 
				{	
					// CONNECTION CLOSED BY CLIENT
					Close(sockfd);
					FD_CLR(sockfd, &allset);
					client[i] = -1;
				} 
				else
				{
					Writen(sockfd, buf, n);
				}

				if (--nready <= 0)
					break;				// NO MORE READABLE DESCRIPTION
			}
			
		}///////////////////////////////////////////////////////
		
		
		
	}/////////////////////////////////////////////////////////////////////////////////////
}

