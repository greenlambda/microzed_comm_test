/*
 * main.c
 *
 *  Created on: Nov 6, 2014
 *      Author: galen
 */
#include <stdlib.h>

#include "common/log.h"
#include "common/util.h"
#include "master/mastermain.h"
#include "submaster/submastermain.h"

#define TAG "main"


int main(int argc, char** argv){
	ProgramParam params;
	int out = 0;


	// parse the program arguments
	if(parseProgramArguments(&params, argc, argv)){
		loge(LOG_ERR, TAG, "Error parsing arguments, program closing\n");
		abort();
	}

	logi(LOG_INF, TAG, "Program Launched\n");

	if(validateProgramArguments(&params)){
		loge(LOG_ERR, TAG, "Failed to validate program arguments, program closing\n");
		abort();
	}

	logi(LOG_DBG, TAG, "Handing control over to sub function for %smaster...\n", params.mode == MASTER ? "" : "sub");
	if(params.mode == MASTER){
		out = masterMain(&params);
	}
	else{
		out = subMasterMain(&params);
	}
	logi(LOG_DBG, TAG, "Regained control from sub function\n");

	logi(LOG_INF, TAG, "Program Closing (status code %d)\n", out);
	return out;
}
