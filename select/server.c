#include	<sys/types.h>
#include	<sys/socket.h>
#include	<sys/time.h>
#include	<time.h>
#include	<netinet/in.h>
#include	<arpa/inet.h>
#include	<errno.h>
#include	<fcntl.h>
#include	<netdb.h>
#include	<signal.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<sys/stat.h>
#include	<sys/uio.h>
#include	<unistd.h>
#include	<sys/wait.h>
#include	<sys/un.h>		
#include	<sys/select.h>	
#include	<sys/sysctl.h>
#include	<poll.h>
#include	<strings.h>
#include	<sys/ioctl.h>
#include	<pthread.h>


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
	
	char 		buffer[1024];
	
	socklen_t	client_len;
	
	struct sockaddr_in	client_addr;
	struct sockaddr_in	server_addr;
	
	listen_FD = socket(AF_INET, SOCK_STREAM, 0);

	bzero(&server_addr, sizeof(server_addr));
	server_addr.sin_family      = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port        = htons(1000);
	
	//printf("AF_FAMILY : %s", server_addr.sin_family);	//DEBUGGING
	//printf("ADDRESS : %s", server_addr.sin_addr.s_addr);	//DEBUGGING
	//printf("PORT : %d", server_addr.sin_port);				//DEBUGGING

	bind(listen_FD, (struct sockaddr *) &server_addr, sizeof(server_addr));

	listen(listen_FD, SOMAXCONN);

	max_FD = listen_FD;										// INITIALISE - 1 FD IS STD INPUT - 2 IS STAD OUTPUT - 3 ALSO IS SOMETHING - 4 IS WHATS NOW AVAILABLE AND WE CONTINUE OF THERE - SO WE KNOW 1,2,3 ARE BY DEFAULT TAKEN BY LINUX AND WE MORE THAN LIKELY ARE 4 NOW. THIS IS THE MAX WE HAVE TO CHECK
	max_i = -1;												// INDEX INTO CLIENT[] ARRAY
	
	for (i = 0; i < FD_SETSIZE; i++) { client[i] = -1; } 	// INDICATE AVAILABLE ENTRY
	
	fd_(&master_set);									//ZERO OUT THE SET
	
	FD_SET(listen_FD, &master_set);							//TELL KERNEL LISTEN TO THIS SET - THE LISTEN FILE DESCRIPTOR


	/////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////
	
	for ( ; ; ) 
	{
	
		copy_set = master_set;			// STRUCTURE ASSIGNMENT - WE COPY FROM MASTER SET AS NOT TO KEEP A SAFE SET 
		
		nready = Select(max_FD + 1, &master_set, NULL, NULL, NULL);

		printf("N READY : %d", nready);//DEBUGGING


		///////////////////////////////////////////////////////
		//CHECK FOR NEW CLIENTS
		
		if (FD_ISSET(listen_FD, &copy_set)) // NEW CLIENT CONNECTION 
		{
			client_len = sizeof(client_addr);
			connection_FD = accept(listen_FD, (struct sockaddr *) &client_addr, &client_len);
			
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
		//CHECK ALL CLIENTS FOR DATA
		
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
		
	}
	
	/////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////
}

