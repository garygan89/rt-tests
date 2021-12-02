/*
 * platform_util.h
 *
 *  Created on: Apr 28, 2016
 *      Author: dsync
 */

#ifndef PLATFORM_UTIL_H_
#define PLATFORM_UTIL_H_

#define _GNU_SOURCE // to set cpu affinity
#include <inttypes.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <linux/mman.h> // Needed for mlockall()
#include <sys/mman.h>	// Needed for mlockall()
#include <malloc.h>
#include <unistd.h>		// needed for sysconf(int name);

#include <sys/time.h>	// needed for getrusage
#include <sys/resource.h>	// needed for getrusage

#include <sched.h> // for cpuset_t struct

#include <errno.h>
#include <asm/msr.h>

int latency_target_fd;
int32_t latency_target_value;

/** Prototypes **/
int platform_set_cpu_affinity(int cpu_core_num);
int platform_disable_lower_pwr_cstate_transition(void);

void platform_mem_show_new_pagefault_count(const char* logtext,
			      const char* allowed_maj,
			      const char* allowed_min);
void platform_mem_prove_thread_stack_use_is_safe(int stacksize);
void platform_mem_configure_malloc_behavior(void);
void platform_mem_reserve_process_memory(int size);





#endif /* PLATFORM_UTIL_H_ */
