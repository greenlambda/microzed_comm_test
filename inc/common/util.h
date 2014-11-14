/*
 * util.h
 *
 *  Created on: Nov 6, 2014
 *      Author: galen
 */

#ifndef UTIL_H_
#define UTIL_H_

#define MSG_OPT_KEEP_CONNECTION_ALIVE (1<<15)




typedef enum program_mode{
	MASTER,
	SUBMASTER,
	UNKNOWN
} ProgramMode;

typedef struct program_param{
	ProgramMode mode;
	int port;
	char *host;
}ProgramParam;




int parseProgramArguments(ProgramParam*params, int argc, char** argv);
int validateProgramArguments(ProgramParam *param);

#endif /* UTIL_H_ */
