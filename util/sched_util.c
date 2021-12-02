#include "sched_util.h"

void set_scheduler() {
	printf("Changing scheduler param prio to 1 and using FIFO scheduling algorithm...\n");
	const struct sched_param priority = {1};
	if ( sched_setscheduler(0, SCHED_FIFO, &priority) == 0 ) {
		printf("Scheduling param set successfully!\n");
	}
	else printf("Scheduling param set failed!\n");

}

void set_scheduler_pthread(int policy, int priority) {
	printf("Changing scheduler param prio to %d and using %s scheduling algorithm...\n",
			priority,
			(policy == SCHED_FIFO) ? "SCHED_FIFO" :
			(policy == SCHED_RR) ? "SCHED_RR" :
			(policy == SCHED_OTHER) ? "SCHED_OTHER" : "???");

	struct sched_param param;

	param.sched_priority = priority;

	pthread_setschedparam(pthread_self(), policy, &param);

}

#define handle_error_en(en, msg) \
               do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)

void display_sched_attr(int policy, struct sched_param *param) {
	printf("    policy=%s, priority=%d\n",
			(policy == SCHED_FIFO) ? "SCHED_FIFO" :
			(policy == SCHED_RR) ? "SCHED_RR" :
			(policy == SCHED_OTHER) ? "SCHED_OTHER" : "???",
			param->sched_priority);
}

/**
 * Display the thread scheduling attributes
 */
void display_thread_sched_attr(char *msg) {
	int policy, s;
	struct sched_param param;

	s = pthread_getschedparam(pthread_self(), &policy, &param);
	if (s != 0)
		handle_error_en(s, "pthread_getschedparam");

	printf("--------------------------------------\n");
	printf("SCHED:\t %s\n", msg);
	display_sched_attr(policy, &param);
	printf("--------------------------------------\n");
}
