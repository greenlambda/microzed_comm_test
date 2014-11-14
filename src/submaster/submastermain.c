/*
 * submastermain.c
 *
 *  Created on: Nov 6, 2014
 *      Author: galen
 */

#include <stdlib.h>
#include <stdbool.h>

#include "submaster/submastermain.h"
#include "common/log.h"
#include "common/util.h"
#include "common/comm.h"

#define TAG "submaster"

int subMasterMain(ProgramParam* params){
	uint8_t *buff;
	MsgContainer container;
	MsgConnection connection;
	// 48 MB of data
	//TODO make this a program param
	size_t size = 1024 * 1024 * 128;

	openConnection(&connection, params->port, params->host);

	buff = malloc(size);

	if(buff != NULL){
		initializeMsg(&container, 0, 0, size, buff);
		logi(LOG_DBG, TAG, "Writing %d bytes to the stream (%.2f Megabytes)...\n", size, size / (1024.0 * 1024));
		sendDataMsg(&connection, &container, false);
		logi(LOG_DBG, TAG, "Done writing to stream\n");
	}
	else{
		loge(LOG_ERR, TAG, "Failed to allocate %d bytes of RAM for buffer, not sending message\n", size);
		return -1;
	}

	logi(LOG_INF, TAG, "Closing connection\n");
	return 0;
}

