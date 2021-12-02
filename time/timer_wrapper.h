/*
 * timer_wrapper.h
 *
 *  Created on: Apr 28, 2016
 *      Author: dsync
 */

#ifndef TIMER_WRAPPER_H_
#define TIMER_WRAPPER_H_

#include <fcntl.h> // for O_RDONLY
#include <errno.h>

/** Timer related interface **/
#include <linux/rtc.h>
#include <sys/timerfd.h>

#include <stdio.h>
#include <linux/ioctl.h>

#include "time_util.h"

#include <sys/ioctl.h>

#define TIMER_TYPE_RTC					0
#define TIMER_TYPE_TIMERABS_NANOSLEEP	1
#define TIMER_TYPE_TIMERFD				2

#define TIMER_STATE_STOP				0
#define TIMER_STATE_RUNNING				1

unsigned char timer_state;

// fd for rtc
int fd;

/** Prototypes **/
int create_rtc_timer(int period_hz, void (*__start_routine) (void *));
int create_timer_fd_timer(int period_us, void (*__start_routine) (void *));

int update_timer_state(unsigned char tstate);
int start_timer();
int stop_timer();

#endif /* TIMER_WRAPPER_H_ */
