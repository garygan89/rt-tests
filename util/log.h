/*
 * log.h
 *
 *  Created on: Mar 12, 2016
 *      Author: dsync
 */

#ifndef LOG_H_
#define LOG_H_

#define INFO 		"INFO"
#define VERBOSE		"VERBOSE"
#define DBG			"DEBUG"
#define WARNING		"WARNING"
#define ERROR		"ERROR"

void print_info(const char *fmt, ...);
void print_debug(const char *fmt, ...);
void print_error(const char *fmt, ...);

#endif /* LOG_H_ */
