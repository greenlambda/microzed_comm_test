/*
 * comm.h
 *
 *  Created on: Nov 13, 2014
 *      Author: galen
 */

#ifndef COMM_H_
#define COMM_H_

#include <netinet/in.h>
#include <stdint.h>
#include <stdbool.h>

#define MAGIC_HEADER  (0xDEADBEEFl)

#define MSG_OPT_KEEP_CONNECTION_ALIVE (1<<15)


typedef struct msg_container{
	uint32_t header;
	uint16_t msgOpts;
	uint16_t msgType;
	uint32_t msgLen;
	uint8_t *buffer;
} MsgContainer;


typedef struct msg_conn{
	bool isAlive;
	int socketfd;
} MsgConnection;

typedef struct server_conn{
	struct sockaddr_in address;
	int socketfd;
} ServerConnection;


void initializeMsg(MsgContainer *container, uint16_t options, uint16_t type, uint32_t length, uint8_t * data);
int openConnection(MsgConnection *connection, int port, char *host);
int closeConnection(MsgConnection *connection);
int sendDataMsg(MsgConnection *connection, MsgContainer *container, bool keepConnectionAlive);
int startServer(ServerConnection *connection, int port, int simoultaniousConnections);
int acceptConnection(MsgConnection *connection, ServerConnection *server);
int readMessage(MsgConnection *connection, MsgContainer *container);
bool isAlive(MsgConnection *connection);


#endif /* COMM_H_ */




























