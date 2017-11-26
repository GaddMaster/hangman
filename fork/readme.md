TEAM 2 - Hangman Forking Server w/ Selecting Client
	Sean Horgan		K00196030
	Daniel Gadd 		K00202350
	Samuel McSweeny 	K00200955

	Server handles multiple clients by splitting new connnections
	into their own processes through a call to fork(). sig_chld
	signals are dealt with by the signal manager in ServerUtility.c,
	uses waitpid().

	Client achieves I/O Multiplexing using a 'do not wait at all'
	waiting method through a call to select().

Source File Info

	* ForkingServer.c - 	Creates a TCP server socket. Listens for 
				connection and forks upon accepting.

	* ServerUtility.c - 	Server utilities placed in their own file
				such as signal handler.

	* PlayHangman.c -	Self explanitory, initiates a game of hangman.

	* Client.c - 		Creates a TCP client socket. Connects to
				server and calls HandleI/O to... well you
				guessed it, handle the I/O.

	* HandleI/O.c - 	Uses select() to achieve I/O multiplexing.
				Handles all I/O between itself & the server.

Header File Info
	
	* AddedStuff.h - 	Contains all #include's and function definitions.

Compile & Run CLIENT

	COMPILE:	gcc -o cli Client.c HandleIO.c
	RUN:		./cli <address> <port>


Compile & Run SERVER


	COMPILE: 	gcc -o srv ForkingServer.c PlayHangman.c ServerUtility.c
	RUN:		./srv

