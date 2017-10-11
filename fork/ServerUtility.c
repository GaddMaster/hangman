/*
 * ------------------------------------------------------
 	Team1: 	Sean Horgan - 		K00196030
		Daniel Gadd - 		
		Samuel McSweeny - 	
 	ServerUtility.c
 	Description:
		Used to store functions used by the
		server.
		- error: a wrapper of perror
		- sigchld_handler: reaps zombies
		- get_in_addr: get IPv4 or IPv6 address 
 * ------------------------------------------------------
 */

 #include "AddedStuff.h"

 /* -- error: A wrapper for perror -- */
 void error (char *msg) {
	perror(msg);
	exit(1);
 }

 /* -- sigchld_handler: handles signals to reap child processes -- */
 void sigchld_handler(int s)
 {
    // waitpid() might overwrite errno, so it has to be restored
    int saved_errno = errno;
    while( waitpid(-1, NULL, WNOHANG) > 0 );
    errno = saved_errno;
 }

 /* -- get sockaddr, IPv4 or IPv6 addresses -- */
 void *get_in_addr(struct sockaddr *sa)
 {
    if ( sa->sa_family == AF_INET ) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
 }
