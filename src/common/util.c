/*
 * util.c
 *
 *  Created on: Nov 6, 2014
 *      Author: galen
 */

#include <getopt.h>
#include <stdlib.h>

#include <stdio.h>
#include "common/log.h"
#include "common/util.h"

#define TAG "util"


void printUsage(char * name){
	char * usage = ""
			"Usage: %s (-m|-s) -p port [-h host] \n"
			"-m\t\tRun in master mode\n"
			"-s\t\tRun in submaster mode\n"
			"-p\t\tSpecify the port to bind to in master mode, and the port to connect to in submaster mode\n"
			"-h\t\tOnly used in submaster mode, specifies the host to connect to\n";
	printf(usage, name);
}

/**
 *
 */
int parseProgramArguments(ProgramParam*params, int argc, char** argv){

	int c;
	char *tailptr;
	opterr = 0;

	if(argc == 1){
		printUsage(argv[0]);
		exit(1);
	}


	// clear out params
	params->mode = UNKNOWN;
	params->host = NULL;
	params->port = 0;

	while ((c = getopt (argc, argv, "msp:h:")) != -1){
		switch (c){
		case 'm':
			// run in master mode
			params->mode = MASTER;
			break;
		case 's':
			// run in submaster mode
			params->mode = SUBMASTER;
			break;
		case 'p':
			params->port = strtol(optarg, &tailptr, 10);
			if(tailptr == optarg || params->port == 0){
				loge(LOG_ERR, TAG, "`%s' is not a valid port number\n", optarg);
				return 1;
			}
			break;
		case 'h':
			params->host = optarg;
			break;
		case '?':
			if (optopt == 'p' || optopt == 'h'){
				loge(LOG_ERR, TAG, "Option -%c requires an argument.\n", optopt);
			}
			else if (isprint (optopt)){
				loge(LOG_ERR, TAG, "Unknown option `-%c'.\n", optopt);
			}else{
				loge(LOG_ERR, TAG, "Unknown option character `\\x%x'.\n", optopt);
				return 1;
			}
			/* no break */
		default:
			abort ();
		}
	}
	return 0;
}

int validateProgramArguments(ProgramParam *param){
	//TODO
	(void) param;

	return 0;
}
