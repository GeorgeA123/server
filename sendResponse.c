/*
 * sendResponse.c
 *
 *  Created on: 26 Oct 2017
 *      Author: george
 */

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include "service_client_socket.h"


#define buffer_size = 1024
int findContentType(char* type, httpHeader* header){
	int len = strlen(header->resource);

	strcpy(type,"Content-Type: ");

	 if (len > 4){
			if (strcmp(&header->resource[len-5], ".html") == 0){

				strcat(type, "text/html");
				strcat(type, ";charset=utf-8");

			}else if (strcmp(&header->resource[len-5], ".jpeg") == 0){
				strcat(type, "image/jpeg");
			}

	}else if (len > 3){
		if (strcmp(&header->resource[len-4], ".css") == 0){
			strcat(type, "text/css");
		}else if (strcmp(&header->resource[len-4], ".xml") == 0){
			strcat(type, "application/xml");
		}else if (strcmp(&header->resource[len-4], ".pdf") == 0){
			strcat(type, "application/pdf");
		}else if (strcmp(&header->resource[len-4], ".gif") == 0){
			strcat(type, "image/gif");
		}else if (strcmp(&header->resource[len-4], ".png") == 0){
			strcat(type, "image/png");
		}

	}

	return 0;
}
int sendHeader(const int socket, httpHeader* header,  size_t size){
	char contentType[1024];

	findContentType(contentType, header);

	fprintf(stderr, "content :  %s\n", contentType);

	int n;
	fprintf(stderr, "socket: %d\n", socket);
	n = send(socket, header->version, strlen(header->version),0);
	fprintf(stderr, "... %d\n", n);
	n = send(socket, " 200 OK \r\n",10, 0);
	n = send(socket, contentType, strlen(contentType),0);
	n = send(socket, "\r\n",2,0);
	n = send(socket, "Content-Length: ",16,0);
	char strSize[256] = "";
	snprintf(strSize, sizeof(strSize), "%zu", size);
	n = send(socket, strSize, strlen(strSize),0);
	n = send(socket, "\r\n",2,0);
	n = send(socket, "\r\n",2,0);

	return 0;

}
int sendResponse(const int socket, httpHeader* header){

	if (header->resource == NULL){
		perror("resource = null");
		return -1;
	}
	char* buffer;
	size_t sizeOfFile = 0;
	char fileURL[1024] = "web";
	strcat(fileURL, header->resource);

	FILE *fp = fopen(fileURL, "r");
	if (fp == NULL){

		if (header->status == 200){

			header->status = 404;
		}
	}else{

		int bufferSize = 0;
		if (fseek(fp,0,SEEK_END) == 0){
			bufferSize =ftell(fp);
		}

		fseek(fp,0, SEEK_SET);
		buffer = malloc(sizeof(char) * bufferSize);
		sizeOfFile = fread(buffer, sizeof(char), bufferSize, fp);

	}
	if (ferror(fp) != 0){
		fprintf(stderr, "error\n");
	}
	fprintf(stderr, "\n%s\n", header->resource);
	fprintf(stderr, "\n%d\n", header->status);
	fprintf(stderr, "\n%s\n", buffer);

	fclose(fp);



	int error = sendHeader(socket, header, sizeOfFile);
	int errorTwo = strlen(buffer);
	while (errorTwo > 0){
		fprintf(stderr, "%d error2\n", errorTwo);
		error = write(socket, buffer, strlen(buffer));
		errorTwo = errorTwo -error;
	}

	fprintf(stderr, "---------END---------");
	return 0;
}
