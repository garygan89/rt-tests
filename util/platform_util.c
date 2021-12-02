/*
 * platform_util.c
 *
 *	Platform memory code is taken from:
 *	https://rt.wiki.kernel.org/index.php/Threaded_RT-application_with_memory_locking_and_stack_handling_example
 *
 *  Created on: Apr 28, 2016
 *      Author: dsync
 */

#include "platform_util.h"

int platform_set_cpu_affinity(int cpu_core_num) {
    //Setting CPU affinity
	int num_CPUs = cpu_core_num; // only run on 1 core
	cpu_set_t mask;

	int numberOfProcessors = sysconf(_SC_NPROCESSORS_ONLN);
	printf("Number of processors: %d\n", numberOfProcessors);

	/* CPU_ZERO initializes all the bits in the mask to zero. */
	CPU_ZERO(&mask);
	/* CPU_SET sets only the bit corresponding to cpu. */
	CPU_SET(num_CPUs, &mask);
	/* sched_setaffinity returns 0 in success */
	if (sched_setaffinity(0, sizeof(mask), &mask) == -1)
	{
	   printf("Could not set CPU Affinity \n");
	   return -1;
	}

	return 0;
}

int platform_disable_lower_pwr_cstate_transition(void)
{
	struct stat s;
	int err, errno;

	errno = 0;
	err = stat("/dev/cpu_dma_latency", &s);
	if (err == -1) {
		perror("WARN: stat /dev/cpu_dma_latency failed");
		return 1;
	}

	errno = 0;
	latency_target_fd = open("/dev/cpu_dma_latency", O_RDWR);
	if (latency_target_fd == -1) {
		perror("WARN: open /dev/cpu_dma_latency");
		return 2;
	}

	errno = 0;
	err = write(latency_target_fd, &latency_target_value, 4);
	if (err < 1) {
		fprintf(stderr, "# error setting cpu_dma_latency to %d!", latency_target_value);
		close(latency_target_fd);
		return 3;
	}
	printf("# /dev/cpu_dma_latency set to %dus\n", latency_target_value);

	return 0;
}

void platform_mem_show_new_pagefault_count(const char* logtext,
			      const char* allowed_maj,
			      const char* allowed_min) {
	static int last_majflt = 0, last_minflt = 0;
	struct rusage usage;

	getrusage(RUSAGE_SELF, &usage);

	printf("%-30.30s: Pagefaults, Major:%ld (Allowed %s), " \
	       "Minor:%ld (Allowed %s)\n", logtext,
	       usage.ru_majflt - last_majflt, allowed_maj,
	       usage.ru_minflt - last_minflt, allowed_min);

	last_majflt = usage.ru_majflt;
	last_minflt = usage.ru_minflt;
}

void platform_mem_prove_thread_stack_use_is_safe(int stacksize)
{
	volatile char buffer[stacksize];
	int i;

	/* Prove that this thread is behaving well */
	for (i = 0; i < stacksize; i += sysconf(_SC_PAGESIZE)) {
		/* Each write to this buffer shall NOT generate a
			pagefault. */
		buffer[i] = i;
	}

	platform_mem_show_new_pagefault_count("Caused by using thread stack", "0", "0");
}

void platform_mem_configure_malloc_behavior(void) {
	/* Now lock all current and future pages
	   from preventing of being paged */
	if (mlockall(MCL_CURRENT | MCL_FUTURE))
		perror("mlockall failed:");

	/* Turn off malloc trimming.*/
	mallopt(M_TRIM_THRESHOLD, -1);

	/* Turn off mmap usage. */
	mallopt(M_MMAP_MAX, 0);
}

void platform_mem_reserve_process_memory(int size)
 {
 	int i;
 	char *buffer;

 	buffer = malloc(size);

 	/* Touch each page in this piece of memory to get it mapped into RAM */
 	for (i = 0; i < size; i += sysconf(_SC_PAGESIZE)) {
 		/* Each write to this buffer will generate a pagefault.
 		   Once the pagefault is handled a page will be locked in
 		   memory and never given back to the system. */
 		buffer[i] = 0;
 	}

 	/* buffer will now be released. As Glibc is configured such that it
 	   never gives back memory to the kernel, the memory allocated above is
 	   locked for this process. All malloc() and new() calls come from
 	   the memory pool reserved and locked above. Issuing free() and
 	   delete() does NOT make this locking undone. So, with this locking
 	   mechanism we can build C++ applications that will never run into
 	   a major/minor pagefault, even with swapping enabled. */
 	free(buffer);
 }
