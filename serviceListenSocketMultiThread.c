/*
 * serviceListenSocketMultiThread.c
 *
 *  Created on: 21 Oct 2017
 *      Author: george
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>

#include <memory.h>
#include <string.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <assert.h>

#include <pthread.h>

#include "service_client_socket.h"
#include "make_printable_address.h"

typedef struct thread_control_block {
  int client;
  struct sockaddr_in6 their_address;
  socklen_t their_address_size;
} thread_control_block_t;


static void * clientThread (void *data) {
  thread_control_block_t *tcb_p = (thread_control_block_t *) data;
  char buffer [INET6_ADDRSTRLEN + 32];
  char *printable;

  assert (tcb_p->their_address_size == sizeof (tcb_p->their_address));

  printable = make_printable_address (&(tcb_p->their_address),
				      tcb_p->their_address_size,
				      buffer, sizeof (buffer));
  (void) service_client_socket (tcb_p->client, printable);
  free (printable);		/* this was strdup'd */
  free (data);			/* this was malloc'd */
  pthread_exit (0);
}

int serviceListenSocketMultiThread(int listenSocket){
	while (1){
		thread_control_block_t *tcbP = malloc(sizeof(thread_control_block_t));
		if (tcbP == 0){
			perror("malloc error");
			return -1;
		}

		int socketFileDiscript;
		tcbP->their_address_size = sizeof(tcbP->their_address);
		printf("%d\n", tcbP->their_address_size);


		if ((socketFileDiscript = accept(listenSocket, (struct sockaddr *) &(tcbP->their_address),
								&(tcbP->their_address_size))) >= 0 ){

			tcbP->client = socketFileDiscript;

			pthread_t thread;

			if (pthread_create(&thread, NULL, clientThread, (void*) tcbP) != 0){
				perror("pthread_create");
				return -1;

			}

			int error = pthread_detach(thread);
			if (error != 0){

				perror("pthread_detach");
				//return -1;
			}
		}else{
			printf("%d\n", socketFileDiscript);
			perror("accept");
		}
	}




}

