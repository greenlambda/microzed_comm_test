#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <sys/timeb.h>
#include "common/log.h"

#define BUF_LEN 1024

static double timer();
static struct timeb start;

void logi(LogLevel level, char *tag, char *format, ...){

	if(strlen(tag) > 11){
		loge(level, "util", "Tag length should not exceed 11 characters\n");
	}

	char line[BUF_LEN];
	va_list args;
	va_start(args, format);

	vsprintf(line, format, args);
	va_end(args);
	fprintf(stdout, "[%08.3f] [%11s] %s", timer(), tag, line);
}

void loge(LogLevel level, char *tag, char *format, ...){
	if(strlen(tag) > 11){
		loge(level, "util", "Tag length should not exceed 11 characters\n");
	}

	char line[BUF_LEN];
	va_list args;
	va_start(args, format);

	vsprintf(line, format, args);
	va_end(args);
	fprintf(stderr, "[%08.3f] [%11s] %s", timer(), tag, line);
}

static double timer(){
	if(start.time == 0)
		ftime(&start);
	struct timeb timer;
	ftime(&timer);

	double diff = timer.time + (timer.millitm / 1000.0);
	diff -= (start.time + (start.millitm / 1000.0));
	return diff;
}
