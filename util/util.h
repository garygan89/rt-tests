/*
 * util.h
 *
 *  Created on: Apr 7, 2016
 *      Author: dsync
 */

#ifndef UTIL_H_
#define UTIL_H_
#define _GNU_SOURCE // to set cpu affinity

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sched.h>
#include <pthread.h>
#include <signal.h>

#include "platform_util.h"

#include <libgen.h> // to eval dir name

#include "log.h"

FILE *f;

//=================================================================
//
// Helper functions to read a JSON file into a malloc'd buffer with '\0' terminator
//
struct FileBuffer{
	unsigned long length;			// length in bytes
	unsigned char *data;			// malloc'd data, free with freeFileBuffer()
};
#define FILE_BUFFER_MAXLEN 1024*1024 // 1MB

typedef struct FileObj {
	FILE *fptr;
	char *file_path;
} FileObj;

unsigned long util_read_file_buffer( char *filename, struct FileBuffer *pbuf, unsigned long maxlen );
void util_free_file_buffer( struct FileBuffer *buf );

#define LOGTAG	"UTIL"

/** Prototypes **/
//FILE * util_create_file(char* filepath);
//int util_close_file(FILE *f);

int util_create_pthread(pthread_t *pthread, void (*__start_routine) (void *), int scheduling_policy, int thread_priority, int thread_stack_size, int cpu_core);
void util_setup_sig_catcher(void (*__signal_handler_routine) (int));


#endif /* UTIL_H_ */
