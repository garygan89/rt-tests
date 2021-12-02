#include "main.h"

/**
 * Routine called by timer
 */
void *read_data_routine_1(void *threadid) {
	printf("t1: Wake up!");
}

void *read_data_routine_2(void *threadid) {
	printf("t2: Wake up!");
}

void *thread_1(void *retcode) {
	printf("Thread1:\t Thread id=#%ld\n", pthread_self());
	set_scheduler_pthread(SCHED_POLICY, PRIO_THREAD_1);
	display_thread_sched_attr("Thread1");

    printf("Creating TimerAbsWakeup...\n");
    create_timer_absolute_nanosleep(TIMER_PERIOD, &read_data_routine_1);

	printf("Thread1:\t Exiting thread1...\n");
// //	pthread_exit(&retcode);
// //	pthread_exit(retcode);
// //	return NULL;

	return (void*) 0;
}

void *thread_2(void *retcode) {
	printf("Thread2:\t Thread id=#%ld\n", pthread_self());
	set_scheduler_pthread(SCHED_POLICY, PRIO_THREAD_2);
	display_thread_sched_attr("Thread2");

    printf("Creating TimerAbsWakeup...\n");
    create_timer_absolute_nanosleep(TIMER_PERIOD, &read_data_routine_2);

	printf("Thread2:\t Exiting thread2...\n");
// //	pthread_exit(&retcode);
// //	pthread_exit(retcode);
// //	return NULL;

	return (void*) 0;
}



int main(int argc, char** argv) {

	/* Disable Paging */
	platform_mem_configure_malloc_behavior();

    /* Assign thread to CPU0 */
	// int nProc = sysconf(_SC_NPROCESSORS_ONLN);
	// printf("Number of processors: %d\n", nProc);
	// cpu_set_t cpus;
	// CPU_ZERO(&cpus);
	// CPU_SET(0, &cpus);
    // pthread_attr_setaffinity_np(&attr1, sizeof(cpu_set_t), &cpus);
    // printf("MAIN:\t Assigned CPU0 to thread-1!\n");    

	int rc = -1;

	//==========================================================================================
	// Thread #1 - Reading Thread
	//==========================================================================================
    rc = util_create_pthread(&(tid[0]), thread_1, SCHED_POLICY, PRIO_THREAD_1, PTHREAD_STACK_MIN, 0);
    if (rc != 0) { print_error("MAIN:\t Error creating thread_1!\n"); }

	//==========================================================================================
	// Thread #2 - Streaming Thread
	//==========================================================================================
    // rc = util_create_pthread(&(tid[1]), thread_2, SCHED_POLICY, PRIO_THREAD_2, PTHREAD_STACK_MIN, 1);
    // if (rc != 0) { print_error("MAIN:\t Error creating thread_2!\n"); }
	

	print_tag("MAIN", "Waiting for threads to join...\n");
    pthread_join(tid[0], &thread_return_status[0]);

    if ((int)thread_return_status[0] == 0) print_tag("MAIN", "Thread1 terminated NORMALLY!\n");
    else print_error("MAIN:\t Thread1 terminated ABNORMALLY!\n");
	

    // pthread_join(tid[1], &thread_return_status[1]);
    // if ((int)thread_return_status[1] == 0) print_tag("MAIN", "Thread2 terminated NORMALLY!\n");
    // else print_error("MAIN:\t Thread2 terminated ABNORMALLY!\n");
	
    print_tag("MAIN", "All done!\n");

}