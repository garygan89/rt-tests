/*
 * time_util.h
 *
 *  Created on: Apr 29, 2016
 *      Author: dsync
 */

#ifndef TIME_TIME_UTIL_H_
#define TIME_TIME_UTIL_H_

#define MICROS_PER_SECOND	1000000
#define NANO_PER_SECOND		1000000000

#include <inttypes.h>
#include <sys/time.h> // for timeval struct

#include<stdio.h>

uint64_t get_realtime_us();
int get_epoch_time(struct timeval *tmv);

#endif /* TIME_TIME_UTIL_H_ */
