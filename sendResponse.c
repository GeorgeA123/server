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


#define buffer_size = 2048
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


	 }
	 if (len > 3){

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
		}else if (strcmp(&header->resource[len-4], ".jpg") == 0){
			strcat(type, "image/jpeg");
		}

	}

	return 0;
}
int sendData (int socket,char* buffer, size_t size){


	int sizeWritten;
	while (size > 0){
		fprintf(stderr, "writing... %s", buffer);
		if ((sizeWritten = write(socket, buffer, size)) < 0){

			return -1;
		}else{

			size -= sizeWritten;
		}
	}
	return size;

}
int sendHeader(const int socket, httpHeader* header,  size_t size){
	char contentType[1024];
	findContentType(contentType, header);
	if (header->version != NULL){
		sendData(socket, header->version, strlen(header->version));
	}else{
		sendData(socket, "HTTP/1.1 ", strlen("HTTP/1.1")); //default to 1.1
	}
	fprintf(stderr, "CONTENT Type: %s\n", contentType);


	if (header->status == 200){

		char strSize[256] = "";
		fprintf(stderr, "size of size: %zd ---", size);
		snprintf(strSize, sizeof(strSize), "%zu", size);
		fprintf(stderr, "size of strSize: %s ---", strSize);
		if (sendData(socket, " 200 OK \r\n", strlen(" 200 OK \r\n")) < 0
				|| sendData(socket, contentType, strlen(contentType)) < 0
				||  sendData(socket, "\r\n", strlen("\r\n")) < 0
				|| sendData(socket, "Content-Length:", strlen("Content-Length")) < 0
				|| sendData(socket, strSize, strlen(strSize)) < 0
				|| sendData(socket, "\r\n", strlen("\r\n")) < 0
				|| sendData(socket, "\r\n",strlen("\r\n")) < 0)
		{
			return -1;
		}
	}else if(header->status == 400){
		if (sendData(socket, " 400 Bad Request\r\n\r\n", strlen(" 400 Bad Request\r\n\r\n")) < 0)
		{
				return -1;
		}
	}else if(header->status == 403){
		if (sendData(socket, " 403 Forbidden\r\n\r\n", strlen(" 403 Forbidden\r\n\r\n")) < 0)
		{
				return -1;
		}
	}else if(header->status == 404){
		strcpy(contentType, "Content-Type: text/html; charset=utf-8");

		if (sendData(socket, " 404 NOT FOUND\r\n", strlen(" 404 NOT FOUND\r\n")) < 0
				|| sendData(socket, contentType, strlen(contentType)) < 0
				|| sendData(socket, "\r\n", strlen("\r\n")) < 0
				|| sendData(socket, "\r\n", strlen("\r\n")) < 0)
		{
			return -1;
		}
	}else if(header->status == 505){
		if (sendData(socket, " 505 Bad Version \r\n", strlen(" 505 Bad Version \r\n")) < 0)
		{
			return -1;
		}
	}
	return 0;

}
int sendBody(const int socket, httpHeader* header, char* buffer, size_t size){
	if (header->status == 200){
		return sendData(socket, buffer, size);
	}else if (header->status ==400){
		return sendData(socket,"400 BAD REQUEST", strlen("400 BAD REQUEST"));
	}else if (header->status == 403){
		return sendData(socket,"403 FORBIDDEN", strlen("403 FORBIDDEN"));
	}else if (header->status == 404){
		return sendData(socket, "404 NOT FOUND", strlen("404 NOT FOUND"));
	}else if (header->status == 505){
		return sendData(socket, "505 BAD VERSION", strlen("505 BAD VERSION"));
	}

	return -1;
}
int sendResponse(const int socket, httpHeader* header){

	if (header->resource == NULL){
		perror("resource = null");
		return -1;
	}
	char* buffer = "";
	size_t sizeOfFile = 0;
	char fileURL[1024] = "web";

	strcat(fileURL, header->resource);

	FILE *fp = fopen(fileURL, "r");
	if (fp == NULL){

		if (header->status == 200){
			header->status = 404;
		}
	}else{

		long bufferSize = 0;
		if (fseeko(fp,0L,SEEK_END) == 0){
			bufferSize =ftello(fp);
		}
		rewind(fp);
		buffer = malloc(sizeof(char) * bufferSize);
		sizeOfFile = fread(buffer, sizeof(unsigned char), bufferSize, fp);
		fprintf(stderr, "Size of File: %zd", sizeOfFile);

	}
	if (fp != NULL){
		if (ferror(fp) != 0){
			fprintf(stderr, "error\n");
		}
		fclose(fp);
	}



	int error = sendHeader(socket, header, sizeOfFile);

	fprintf(stderr, "error: %d", error);
	if (error > 0){
		fprintf(stderr,"not 200\n\n");

	}
	if (strcmp(buffer, "") == 0 && header->status == 200){
		header->status = 404;
	}
	fprintf(stderr, "LENGTH OF BUFFER %d", strlen(buffer) );
	long sent = sendBody(socket, header, buffer, sizeOfFile);
	fprintf(stderr, "sent : %d \n", sent);

	fprintf(stderr, "---------END---------");
	close(socket);
	return 0;
}
