/*
 * -----------------------------------------------------------------------------
 	Team1: 	Sean Horgan - 		K00196030
		Daniel Gadd - 		K00202350
		Samuel McSweeny - 	
 	
	Name:	ForkingServer.c

 	Description:
		Creates a listeing socket. Accepts
		incoming connections from client.
		Once connection is accepted, fork().
		Once forked, play Hangman by calling
		run_process() function in run_process.c.

	Compile: 
		gcc -o srv ForkingServer.c run_process.c ServerUtility.c
 * -----------------------------------------------------------------------------
 */

 # include "AddedStuff.h"

 int main(int argc, char **argv)
 {
	struct addrinfo hints, *servinfo, *p;	/* Used with getaddrinfo */
	struct sockaddr_in cliaddr, servaddr;	/* Store cli and srv address */
	struct sigaction sa;			/* For signal handling */
	int listenfd, connfd; 		/* listening and connection socket */
	int clntPort;
	int rv;
	int yes=1;
	char s[INET6_ADDRSTRLEN];
	pid_t childpid;			/* child process ID */
	socklen_t clilen;		/* bytesize of client's adress min */
    	memset(&hints, 0, sizeof hints);
	
	/* -- socket: create a socket -- */	
	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if ( listenfd < 0 )
		error("ERROR opening socket");

	/* -- Build the servers inet address -- */
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
    	hints.ai_socktype = SOCK_STREAM;
    	hints.ai_flags = AI_PASSIVE; // use my IP
    	if ( (rv = getaddrinfo(NULL, SERV_PORT_STR, &hints, &servinfo)) != 0 ) {
        	fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        	return 1;
    	}

	/* -- bind: associate the listening socket with a port -- */
	/* loop through all the results and bind to the first we can */
    	for(p = servinfo; p != NULL; p = p->ai_next) {
        	if ( (listenfd = socket(p->ai_family, p->ai_socktype,
                	p->ai_protocol)) == -1 ) {
            		error("server: socket");
            		continue;
        	}
        	if ( setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &yes,
                	sizeof(int)) == -1 ) {
            		error("setsockopt");
            		exit(1);
        	}
       		if ( bind(listenfd, p->ai_addr, p->ai_addrlen ) == -1) {
            		close(listenfd);
           	 	error("server: bind");
            		continue;
        	}
        	break;
    	}
    	freeaddrinfo(servinfo); /* all done with this structure */
    	if ( p == NULL )  {
        	fprintf(stderr, "server: failed to bind\n");
        	exit(1);
    	}
	
	/* listen: make it a listening socket, read to accept connection reqs */
	if ( listen(listenfd, BACKLOG) == -1 ) {/* Allow 10 requests to queue */
        	error("listen");
        	exit(1);
    	}
	else {
		printf("STATUS: Listening for connection...\n");
	}
	
	/* -- Reap any Zombie Processes -- */
	sa.sa_handler = sigchld_handler; /* Specify signal catching function */
    	sigemptyset(&sa.sa_mask);	/* Initialise and empty signal set */
    	sa.sa_flags = SA_RESTART; /* Ensure function restarts after interrupt*/
    	if ( sigaction(SIGCHLD, &sa, NULL) == -1 ) { /* Run sigaction */
        	error("sigaction");
        	exit(1);
    	}

	/* -- main loop: wait for connection request, accept & call fork() -- */	
	for ( ; ; ) {
	    clilen = sizeof(cliaddr);
	    connfd = accept(listenfd, (struct sockaddr *) &cliaddr, &clilen);
	    if ( listenfd == -1 ) {
            	error("accept");
            	continue;
            }
		
	    /* Get Client port for printing */
	    clntPort = ntohs(cliaddr.sin_port);

	    /* Convert address into text form for representation */
	    if(inet_ntop(cliaddr.sin_family,
            	get_in_addr((struct sockaddr *)&cliaddr), s, sizeof s) != NULL){
		printf("------------------------------------\n");		
		printf("Established connection: \n");
		printf("Client Address: %s\n", s);
		printf("Client Port: 	%d\n", clntPort);
	    }//end if     
	    else{
      		puts("Unable to get client address");
	    }
            printf("STATUS: Calling Fork...\n");

	    /* -- fork: create a child process that handles new connection -- */
	    if ( (childpid = fork()) == 0 ) { 	/* child process */
		/* Print that fork succeeded with child process ID */
		printf("STATUS: Fork succeeded!\nProcess ID: %d\n", getpid());
		close(listenfd);		/* close listening sock */
		run_process(connfd, connfd);	/* process the request */
		exit (0);
	    } // end if

	    close(connfd);		/* parent closes connected sock */

	} // end for
} // end main
