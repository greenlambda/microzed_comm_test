/*
 * mastermain.c
 *
 *  Created on: Nov 6, 2014
 *      Author: galen
 */

#include <stdlib.h>
#include <stdbool.h>

#include "master/mastermain.h"
#include "common/util.h"
#include "common/log.h"
#include "common/comm.h"

#define TAG "mastermain"

int masterMain(ProgramParam *params){
	int out = 0;
	ServerConnection server;
	MsgConnection conn;
	MsgContainer container;

	logi(LOG_DBG, TAG, "Starting server on port %d...\n", params->port);
	startServer(&server, params->port, 3);
	logi(LOG_DBG, TAG, "Server starting\n");

	while(1){

		logi(LOG_DBG, TAG, "Waiting for client connection...\n");
		acceptConnection(&conn, &server);
		logi(LOG_DBG, TAG, "Got client connection\n");

		while(isAlive(&conn)){
			logi(LOG_DBG, TAG, "Reading message from connection...\n");
			out = readMessage(&conn, &container);
			logi(LOG_DBG, TAG, "Read message from connection %ssuccessfully\n", out == 0 ? "" : "un");

			if(container.buffer != NULL){
				free(container.buffer);
			}
		}

		logi(LOG_DBG, TAG, "Client connection closed\n");
	}


	return out;
}





