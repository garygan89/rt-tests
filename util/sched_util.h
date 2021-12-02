#ifndef SCHED_UTIL_H_
#define SCHED_UTIL_H_

#include <stdlib.h> // for EXIT_FAILURE
#include <errno.h> // for errno

#include <sched.h>

void set_scheduler(void);

void set_scheduler_pthread(int policy, int priority);

void display_sched_attr(int policy, struct sched_param *param);

void display_thread_sched_attr(char *msg);

#endif
