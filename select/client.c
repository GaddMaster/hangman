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

 int main (int argc, char * argv [])
 {
 	int cli_sock;
 	char * server_name;

	struct hostent * host_info;
 	struct sockaddr_in server_addr; // SERVER ADDRESS

 	// GET SERVER NAME FROM COMMAND LINE ELSE USE LOCALHOST
 	server_name = (argc = 1)?  argv [1]: "localhost";

 	// CREATE A SOCKET
 	cli_sock = socket(AF_INET, SOCK_STREAM, 0);
 	if (cli_sock <0) {
 		perror ("Creating stream socket");
 		exit (1);
 	}

 	host_info = gethostbyname(server_name);
 	if (host_info == NULL) {
 		//fprintf (stderr, "%s: unknown host:%s \ n", argv [0], server_name);
 		exit (2);
 	}


 	// SET UP SERVER SOCKET ADDRESS AND CONNECT
 	server_addr.sin_family = host_info->h_addrtype;
 	memcpy ((char *) & server_addr.sin_addr, host_info->h_addr, host_info->h_length);
 	server_addr.sin_port = htons (1000);

 	if (connect(cli_sock, (struct sockaddr *) & server_addr, sizeof server_addr) <0) {
 		perror ("connecting to server");
 		exit (3);
 	}
 	

 	printf("Connected to server : %s \n", server_name);
 	
 	char * message = "Hello Server\n";
 	
	send(cli_sock, message, sizeof(message), 0);
	
	close(cli_sock);
	
	return 0;
 }
