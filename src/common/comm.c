/*
 * comm.c
 *
 *  Created on: Nov 13, 2014
 *      Author: galen
 */

#include <netdb.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <netinet/in.h>

#include <sys/socket.h>
#include <sys/timeb.h>
#include <sys/types.h>

#include "common/comm.h"
#include "common/log.h"

#define TAG "comm lib"

/*
 * Deal with the fact that receiving can take multiple calls.
 */
ssize_t recvSafe(int sockfd, void *buf, size_t len, int flags) {
	// ssize_t retValue;
	// ssize_t totalLen = len;
	// logi(LOG_DBG, TAG, "Reading %d bytes.\n", totalLen);
	// while (len > 0) {
	return recv(sockfd, buf, len, flags);
	// 	if (retValue < 0) {
	// 		return -1;
	// 	}
	// 	logi(LOG_DBG, TAG, "  Sub Read %d bytes.\n", retValue);
	// 	len -= retValue;
	// 	buf += retValue;
	// }
	// return totalLen;
}

void initializeMsg(MsgContainer *container, uint16_t options, uint16_t type, uint32_t length, uint8_t * data){
	if(container != NULL){
		container->header = MAGIC_HEADER;
		container->buffer = data;
		container->msgLen = length;
		container->msgOpts = options;
		container->msgType = type;
	}
}

int openConnection(MsgConnection *connection, int port, char *host){
	int sockfd;
	struct sockaddr_in serv_addr;
	struct hostent *server;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (sockfd < 0){
		return -1;
	}

	server = gethostbyname(host);

	if (server == NULL) {
		fprintf(stderr, "ERROR: Failed to lookup host `%s'\n", host);
		return -1;
	}

	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;

	bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
	serv_addr.sin_port = htons(port);

	if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0){
		perror("Connect failed");
		return -1;
	}

	connection->socketfd = sockfd;
	return 0;
}

int closeConnection(MsgConnection *connection){
	connection->isAlive = false;
	close(connection->socketfd);
	return 0;
}

int sendDataMsg(MsgConnection *connection, MsgContainer *container, bool keepConnectionAlive){
	int returnCode;
	unsigned int written = 0;
	//TODO proper error handling

	if(keepConnectionAlive){
		// set the keep connection alive bit to 1
		container->msgOpts |= (MSG_OPT_KEEP_CONNECTION_ALIVE);
	}
	else{
		// set the keep connection alive bit to 0
		container->msgOpts &= (~MSG_OPT_KEEP_CONNECTION_ALIVE);
	}

	// order of packets:
	// message magic header
	// message options
	// message type
	// message data length (does not include header)
	// message data
	logi(LOG_DBG, TAG, "Sending Header %lX.\n", container->header);
	returnCode = write(connection->socketfd, &container->header, sizeof(container->header));
	if(returnCode < 0){
		return -1;
	}

	logi(LOG_DBG, TAG, "Sending Opts %d.\n", container->msgOpts);
	returnCode = write(connection->socketfd, &container->msgOpts, sizeof(container->msgOpts));
	if(returnCode < 0){
		return -1;
	}

	logi(LOG_DBG, TAG, "Sending Type %d.\n", container->msgType);
	returnCode = write(connection->socketfd, &container->msgType, sizeof(container->msgType));
	if(returnCode < 0){
		return -1;
	}

	logi(LOG_DBG, TAG, "Sending Length %d.\n", container->msgLen);
	returnCode = write(connection->socketfd, &container->msgLen, sizeof(container->msgLen));
	if(returnCode < 0){
		return -1;
	}

	while(written < container->msgLen){
		returnCode = write(connection->socketfd, &(container->buffer[written]), container->msgLen - written);

		if(returnCode < 0){
			return -1;
		}

		written += returnCode;
	}

	if(!keepConnectionAlive){
		closeConnection(connection);
	}

	return 0;
}

int startServer(ServerConnection *connection, int port, int simoultaniousConnections){
	int socket_desc;

	socket_desc = socket(AF_INET, SOCK_STREAM, 0);

	if (socket_desc < 0){
		return -1;
	}

	/* type of socket created in socket() */
	connection->address.sin_family = AF_INET;
	connection->address.sin_addr.s_addr = INADDR_ANY;
	connection->address.sin_port = htons(port);

	/* bind the socket to the port specified above */
	if(bind(socket_desc,(struct sockaddr *)&(connection->address), sizeof(connection->address)) < 0){
		perror("Failed to bind to port");
		return -1;
	}

	listen(socket_desc, simoultaniousConnections);
	connection->socketfd = socket_desc;
	return 0;
}

int acceptConnection(MsgConnection *connection, ServerConnection *server){
	unsigned int addrlen;
	int new_socket;
	addrlen = sizeof(struct sockaddr_in);
	new_socket = accept(server->socketfd, (struct sockaddr *) &(server->address), &addrlen);

	if (new_socket < 0){
		return -1;
	}
	connection->isAlive = true;
	connection->socketfd = new_socket;
	return 0;
}

int readMessage(MsgConnection *connection, MsgContainer *container){
	struct timeb start;
	struct timeb end;
	int socket = connection->socketfd;
	int returnCode;

	bzero(container, sizeof(&container));

	// read the header and make sure the magic number lines up
	returnCode = recvSafe(connection->socketfd, &(container->header), sizeof(container->header), 0);

	if(returnCode <= 0){
		logi(LOG_DBG, TAG, "Bad return code...\n");
		return -1;
	}

	logi(LOG_DBG, TAG, "Got %lX. Expected %lX.\n", container->header, MAGIC_HEADER);
	if(container->header != MAGIC_HEADER){
		
		logi(LOG_DBG, TAG, "Bad magic header...\n");
		return -1;
	}

	// read in the message options
	if(recvSafe(socket, &container->msgOpts, sizeof(container->msgOpts), 0) < 0){
		logi(LOG_DBG, TAG, "Bad message options...\n");
		return -1;
	}
	logi(LOG_DBG, TAG, "Options %d.\n", container->msgOpts);

	// read in the type of the message
	if(recvSafe(socket, &container->msgType, sizeof(container->msgType), 0) < 0){
		logi(LOG_DBG, TAG, "Bad message type...\n");
		return -1;
	}
	logi(LOG_DBG, TAG, "Msg Type %d.\n", container->msgType);

	// read in the size of the message
	if(recvSafe(socket, &container->msgLen, sizeof(container->msgLen), 0) < 0){
		logi(LOG_DBG, TAG, "Bad message size...\n");
		return -1;
	}
	logi(LOG_DBG, TAG, "Reading %d bytes...\n", container->msgLen);

	container->buffer = malloc(container->msgLen);

	if(container->buffer == NULL){
		logi(LOG_DBG, TAG, "Failed to malloc space for data...\n");
		return -1;
	}

	long read = 0;
	long toRead = container->msgLen;
	ftime(&start);
	returnCode = 0;
	while (toRead > 0) {
		returnCode = recv(socket, container->buffer, toRead, 0);
		read += returnCode;
		toRead -= returnCode;
	}
	ftime(&end);

	long timeInMilliseconds = ((end.time * 1000) + end.millitm) - ((start.time * 1000) + start.millitm);
	logi(LOG_INF, TAG, "Received %d bytes in %ld milliseconds (%.2f Megabytes/sec)\n", container->msgLen, timeInMilliseconds, (container->msgLen / (1024.0 * 1024)) / (timeInMilliseconds / 1000.0));

	// if we aren't supposed to keep the connection alive, close it
	if((container->msgOpts & MSG_OPT_KEEP_CONNECTION_ALIVE) == 0){
		closeConnection(connection);
	}

	return 0;
}

bool isAlive(MsgConnection *connection){
	return connection->isAlive;
}
