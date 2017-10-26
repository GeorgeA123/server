/*
 * getListenSocket.c
 * Create and return a socket which can listen
 * created on: 15 Oct 2017
 *
 */

#include <stdio.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <errno.h>

/*
 * give a port, return a socket to listen on
 */
int getListenSocket(int port){
	//create socket with IPv6
	struct sockaddr_in6 address;

	memset (&address, '\0', sizeof(address));

	address.sin6_port = htons(port);
	address.sin6_addr = in6addr_any;
	address.sin6_family = AF_INET6;

	/*https://stackoverflow.com/questions/6729366/
	 *what-is-the-difference-between-af-inet-and-pf-inet-in-socket-programming
	 *PF_INET > AF_INET
	 */
	int listenSocket = socket(PF_INET6, SOCK_STREAM,0);

	if (listenSocket < 0){
		perror("socket");
		return -1;
	}

	const int one = 1;
	if (setsockopt (listenSocket, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one)) != 0){
		perror("setsocket");
		//not ending program due to it not being necessary
	}

	if (bind(listenSocket, (struct sockaddr *) &address, sizeof(address)) != 0){
		perror("bind");
		return -1;
	}

	if (listen(listenSocket,5)!=0){
		perror("listen");
		return -1;
	}

	return listenSocket;

}


