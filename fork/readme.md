=
Compile & Run CLIENT

	COMPILE:	gcc -o cli Client.c HandleIO.c
	RUN:		./cli <address> <port>


=
Compile & Run SERVER


	COMPILE: 	gcc -o srv ForkingServer.c PlayHangman.c ServerUtility.c
	RUN:		./srv

