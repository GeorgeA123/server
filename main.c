/*
 * main.c
 *
 *  Created on: 20 Oct 2017
 *      Author: george
 */

#include <stdio.h>
#include <stdlib.h>

#include "getListenSocket.h"
#include "serviceListenSocketMultiThread.h"
int main(int argc, char **argv){

	if (argc != 2){
		printf("Error, incorrect usage, please enter port: \n");
		exit(1);
	}
	char *endPoint;
	//get port number from the arguments
	int port = strtol(argv[1], &endPoint,10);

	printf("port number is: %d \n", port);

	//error checking, is it a number? is it a legit number?
	if (*endPoint != '\0'){
		printf("%s is not a number, please enter a number\n", argv[1]);
		exit(1);
	}
	if (port > 65535){
		printf("%d.The port number needs to be less than 65535\n", port);
		exit(1);
	}
	//get a listening socket
	int listenSocket = getListenSocket(port);
	printf("%d", listenSocket);

	if (listenSocket < 0){
		printf("%s"," could not create a socket\n");
		exit(1);
	}
	//handle the acquired socket
	if (serviceListenSocketMultiThread(listenSocket) != 0){
		printf("%s: could not process the socket\n");
		exit(1);
	}


	return 0;
}


