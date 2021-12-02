#define _GNU_SOURCE // for cpu set affinity
#define __STDC_FORMAT_MACROS

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <signal.h>
#include <pthread.h>
#include <sched.h>
#include <errno.h>
#include <limits.h>

#include <inttypes.h>
#include <math.h>

#include "util/platform_util.h"
#include "util/sched_util.h"
#include "util/util.h"

#include "time/time_util.h"
#include "time/timer_wrapper.h"

/** Scheduling **/
#define SCHED_POLICY 								SCHED_FIFO
#define PRIO_THREAD_1 						        90
#define PRIO_THREAD_2 					            90


/** Timer **/
/** Timer Type **/
#define DEFAULT_TIMER_TYPE 							TIMER_TYPE_TIMERABS_NANOSLEEP
//#define TIMER_PERIOD 								50 // micros
#define TIMER_PERIOD 								500 // micros

#define DEFAULT_PERIODIC_TIMER_FREQUENCY 2048 // how often to read DATA_READY pin from ADS for interrupt

/** Threads **/
#define NUM_THREADS                                 2

pthread_t tid[NUM_THREADS];
/* status code return by the spawned thread, 0 = normal exit */
void *thread_return_status[NUM_THREADS] = {0};
