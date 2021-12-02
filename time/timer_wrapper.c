/*
 * timer_wrapper.c
 *
 *  Created on: Apr 28, 2016
 *      Author: dsync
 */

#include "timer_wrapper.h"

int get_timer_state() {
	return timer_state;
}

int update_timer_state(unsigned char tstate) {
	timer_state = tstate;
	return 0;
}

int start_timer() {
	printf("TIMER_WRAPPER:\t Starting timer...\n");
	update_timer_state(TIMER_STATE_RUNNING);
	return 0;
}

int stop_timer() {
	printf("TIMER_WRAPPER:\t Stopping timer...\n");
	update_timer_state(TIMER_STATE_STOP);
	return 0;
}

int create_rtc_timer(int period_hz, void (*__start_routine) (void *)) {
	int i, retval, irqcount = 0;
	unsigned long tmp, data;
	struct rtc_time rtc_tm;

	long long ts0, ts1;
	long now;

	int irqs;

	fd = open("/dev/rtc0", O_RDONLY);

	if (fd == -1) {
		perror("/dev/rtc");
		exit(errno);
	}

	// set the periodic timer hz
	retval = ioctl(fd, RTC_IRQP_SET, period_hz);
	if (retval == -1) {
		perror("ioctl");
		exit(errno);
	}

	/* Read periodic IRQ rate */
	retval = ioctl(fd, RTC_IRQP_READ, &tmp);
	if (retval == -1) {
		perror("ioctl");
		exit(errno);
	}
	printf("Periodic IRQ rate is %ldHz.\n", tmp);

//	printf("%dHz:\n", periodic_timer_hz);

	/* Enable periodic interrupts */
	retval = ioctl(fd, RTC_PIE_ON, 0);
	if (retval == -1) {
		perror("ioctl");
		exit(errno);
	}

	start_timer();

	do {
		/* This blocks */
		retval = read(fd, &data, sizeof(unsigned long));

		if (retval == -1) {
			perror("read");
			exit(errno);
		}

		__start_routine(0);

		irqcount++;

		// check irq pile up
		irqs=data>>8;
//		printf("IRQ=%d\n", irqs);
		if(irqs>1) printf("IRQ > 1, irqCount=%ld\n ",irqs);fflush(stdout);

	} while (timer_state == TIMER_STATE_RUNNING);

	/* Disable periodic interrupts */
	printf("TIMER_WRAPPER:\t RTC Periodic interrupt disabled!\n");
	retval = ioctl(fd, RTC_PIE_OFF, 0);
	if (retval == -1) {
		perror("ioctl");
		exit(errno);
	}

	return 0;
}

/**
 * Creating timer fd
 * 2-Step process:
 *
 * 1. Setup timer fd
 * 2. Setup periodic rate
 */
int create_timer_fd_timer(int period_us, void (*__start_routine) (void *)) {
	// Step 1
	int ret;
	unsigned int ns;
	unsigned int sec;
	int fd;
	struct itimerspec itval;

	int wakeup_missed = 0; // how many times interrupt is missed

	/* Create the timer */
	fd = timerfd_create(CLOCK_MONOTONIC, 0);
//	info->wakeups_missed = 0;
//	info->timer_fd = fd;
	if (fd == -1)
		return fd;

	/* Make the timer periodic */
	sec = period_us / MICROS_PER_SECOND;
	ns = (period_us - (sec * MICROS_PER_SECOND)) * 1000;
	itval.it_interval.tv_sec = sec;
	itval.it_interval.tv_nsec = ns;
	itval.it_value.tv_sec = sec;
	itval.it_value.tv_nsec = ns;
	timerfd_settime(fd, 0, &itval, NULL);

	// Step 2
	unsigned long long missed;

	start_timer();

	do {
		/* Wait for the next timer event. If we have missed any the
		   number is written to "missed" */
		ret = read (fd, &missed, sizeof (missed));
		if (ret == -1) {
			perror ("read timer");
			return 1;
		}

		/* "missed" should always be >= 1, but just to be sure, check it is not 0 anyway */
		if (missed > 0)
			wakeup_missed += (missed - 1);
//			info->wakeups_missed += (missed - 1);

		__start_routine(0);
	} while (timer_state == TIMER_STATE_RUNNING);

	printf("TimerFD:\t Wakeup missed = %d\n", wakeup_missed);

	return 0;
}

int create_timer_absolute_nanosleep(int period_us, void (*__start_routine) (void *)) {
	struct timespec ts;

	clock_gettime(CLOCK_MONOTONIC, &ts);
	ts.tv_sec += period_us / MICROS_PER_SECOND;
	ts.tv_nsec += period_us * 1000;

	start_timer();

	do {
		clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &ts, NULL);
		__start_routine(0);
		ts.tv_sec += period_us / MICROS_PER_SECOND;
		ts.tv_nsec += period_us * 1000;
		if(ts.tv_nsec >= NANO_PER_SECOND) {
			ts.tv_nsec -= NANO_PER_SECOND;
			ts.tv_sec++;
		}


	} while (timer_state == TIMER_STATE_RUNNING);

	return 0;
}
