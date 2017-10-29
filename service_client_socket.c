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

#include <stdbool.h>
#include <assert.h>

#include "service_client_socket.h"
#include "sendResponse.h"
/* why can I not use const size_t here? */
#define buffer_size 1024


static int stringToHeader(httpHeader *header,char* buffer, size_t length){
	/* add null terminal rather than \r	 */
	buffer[length - 1] = '\0';
	/* if there is no version stored and the length > 4
	 * then we can assume the first line is the resource
	 */
	header->status = 200;
	if (header->version == NULL && header->resource == NULL && length > 4){
		//find the method
		if (strncmp(buffer, "GET ", 4) == 0){
			header->method = GET;
		}else if(strncmp(buffer, "HEAD ",5) == 0){
			header->method = HEAD;
		}else{
			header->method = UNSUPPORTED;
			//something bad has happened
		}

		strtok(buffer, " ");

		char *resource = strtok(NULL, " ");

		char *version = strtok(NULL, " ");
				/*version*/
		if (strcmp(version, "HTTP/1.1") == 0 || strcmp(version, "HTTP/1.0") == 0 ){
			header->version = calloc(1+ strlen(version), sizeof(char));
			strcpy(header->version, version);
		}else if (version == NULL){
			header->status = 400;
		}else{
			header->status = 505;
		}
				/*resource*/
		if (resource != NULL){
			if (strcmp(resource, "/") == 0){

				strcat(resource, "index.html");
			}
			header->resource = calloc(1 + strlen(resource), sizeof(char));
			strcpy(header->resource, resource);
		}else{
			//bad request

			header->status = 400;
			return -1;
		}

	}else{
		if (strncmp(buffer, "User-Agent:", 11) == 0){
			strtok(buffer, " ");
			char* ag = strtok(NULL, " ");

			header->userAgent = calloc(1 + strlen(ag), sizeof(char));
			strcpy(header->userAgent, ag);


		}

	}


	/* give it back as it was */
	buffer[length - 1] = '\r';
	return 0;

}
static int readHeader(const int s, httpHeader *header){
	char buffer[buffer_size];
	size_t bytes;

	/* need to find lines to attach them to httpHeader struct */
	size_t endIndex =0;
	bool recentNewLine = false;

	while((bytes= read(s, &buffer[endIndex],  1)) > 0){
		if (endIndex >= 1 && buffer[endIndex -1] == '\r' && buffer[endIndex] == '\n'){
			if (recentNewLine){
				//eof
				return 0;
			}

			//fill in the correct httpHeader variable
			stringToHeader(header, buffer, endIndex);

			recentNewLine = true;
			endIndex = 0;
		}else{
			endIndex ++;
			recentNewLine = !(endIndex > 1);

			if (endIndex > buffer_size){
				printf("header to big");
				return -1;
			}
		}


	}
	if (bytes != 0){
		perror ("read");
		return -1;
	}
	return -1;

}
int service_client_socket (const int s, const char *const tag) {

	printf("Connected, %s \n", tag);
	httpHeader *header = malloc(sizeof(httpHeader));
	header->method = UNSUPPORTED;
	header->resource = NULL;
	header->version = NULL;
	header->userAgent = NULL;
	header->status =-1;


	int error = readHeader(s, header);



	if (error != 0){
		header->status = 400;
		header->version = calloc(strlen("HTTP/1.1")+1, sizeof(char));
		strcpy(header->version , "HTTP/1.1");

	}else if(header->method == UNSUPPORTED){
		header->status = 501;
	}
	/*
	fprintf(stderr, "stored data\nversion: %s\n", header->version);
	fprintf(stderr,"resource: %s\n", header->resource);
	fprintf(stderr,"userAgent: %s\n", header->userAgent);
	fprintf(stderr,"status: %d\n", header->status);
	fprintf(stderr,"method: %d\n", header->method);
*/
	error = sendResponse(s, header);
/*
#if (__SIZE_WIDTH__ == 64 || __SIZEOF_POINTER__  == 8)
	printf("echoed %ld bytes back to %s, \"%s\"n", bytes, tag, buffer);
#else
	printf("echoed %d bytes back to %s, \"%s\"n", bytes, tag, buffer);
#endif

*/

	return 0;
}

