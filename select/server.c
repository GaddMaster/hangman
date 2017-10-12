
#include	"unp.h"

int main(int argc, char **argv)
{
	int			i;
	int			max_i;
	int 		max_FD;
	int 		listen_FD;
	int 		connection_FD;
	int 		socket_FD;
	int			nready;
	int			client[FD_SETSIZE];
	
	ssize_t		n;
	ssize_t 	FD_set;			
	ssize_t 	copy_set;
	ssize_t 	master_set;
	
	char 		buffer[MAXLINE];
	
	socklen_t	client_len;
	
	struct sockaddr_in	client_addr;
	struct sockaddr_in	server_addr;
	
	listen_FD = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&server_addr, sizeof(server_addr));
	server_addr.sin_family      = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port        = htons(SERV_PORT);

	Bind(listen_FD, (SA *) &server_addr, sizeof(server_addr));

	Listen(listen_FD, LISTENQ);

	max_FD = listen_FD;			// INITIALISE
	max_i = -1;					// INDEX INTO CLIENT[] ARRAY
	
	for (i = 0; i < FD_SETSIZE; i++) { client[i] = -1; } // INDICATE AVAILABLE ENTRY
	
	FD_ZERO(&master_set);
	
	FD_SET(listen_FD, &master_set);


	/////////////////////////////////////////////////////////////////////////////////////
	for ( ; ; ) 
	{
	
		copy_set = master_set;			// STRUCTURE ASSIGNMENT
		
		nready = Select(max_FD + 1, &master_set, NULL, NULL, NULL);




		///////////////////////////////////////////////////////
		
		if (FD_ISSET(listen_FD, &rset)) // NEW CLIENT CONNECTION
		{
			client_len = sizeof(client_addr);
			connection_FD = Accept(listen_FD, (SA *) &client_addr, &client_len);
			
			#ifdef	NOTDEF
				printf("NEW CLIENT: %s, PORT %d\n", Inet_ntop(AF_INET, &client_addr.sin_addr, 4, NULL),ntohs(client_addr.sin_port));
			#endif

			for (i = 0; i < FD_SETSIZE; i++)
			{
				if (client[i] < 0) 
				{
					client[i] = connection_FD;	// SAVE DESCRIPTOR
					break;
				}
			}
			
			if (i == FD_SETSIZE) { err_quit("TOO MANY CLIENTS"); }		//EDN PROGRAM AS TOO MANY CLIENTS

			FD_SET(connection_FD, &master_set);							// ADD NEW DESCRIPTOR TO SET
			
			if (connection_FD > max_FD) { max_FD = connection_FD; }		// FOR SELECT
			
			if (i > max_i) { max_i = i; }								// MAX INDEX IN CLIENT[] ARRAY

			if (--nready <= 0) { continue; }							// NO MORE READABLE DESCRIPTOR
		}
		
		////////////////////////////////////////////////////////
		



		////////////////////////////////////////////////////////
		
		for (i = 0; i <= maxi; i++) 	// CHECK ALL CLIENT FOR DATA
		{
			if ( (socket_FD = client[i]) < 0) { continue; }
				
			if (FD_ISSET(socket_FD, &copy_set)) 
			{
				if ( (n = Read(socket_FD, buffer, MAXLINE)) == 0) 
				{	
					// CONNECTION CLOSED BY CLIENT
					Close(socket_FD);
					FD_CLR(socket_FD, &master_set);
					client[i] = -1;
				} 
				else{Writen(socket_FD, buffer, n);}

				if (--nready <= 0) { break; }							// NO MORE READABLE DESCRIPTION
			}
		}
		
		///////////////////////////////////////////////////////
		
		
		
	}/////////////////////////////////////////////////////////////////////////////////////
}

