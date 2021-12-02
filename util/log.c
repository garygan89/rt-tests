/*
 * log.c
 *
 *  Created on: May 20, 2016
 *      Author: dsync
 */

#include <stdio.h>
#include <stdarg.h>
#include "../util/log.h"

#include "../ansi/ansi-color.h"

/**
 * Print a custom tag message
 * E.g.
 * MAIN: abc
 */
void print_tag(char *tag, const char *fmt, ...) {
	va_list args;
	va_start (args, fmt);
	printf("%s:\t", tag);
	vprintf(fmt, args);
	va_end (args);
}

void print_info(const char *fmt, ...) {
	va_list args;
	va_start (args, fmt);
	printf("%s:\t", INFO);
	vprintf(fmt, args);
	va_end (args);
}

void print_verbose(const char *fmt, ...) {
	va_list args;
	va_start (args, fmt);
	printf("%s:\t", VERBOSE);
	vprintf(fmt, args);
	va_end (args);
}

void print_debug(const char *fmt, ...) {
	va_list args;
	va_start (args, fmt);
	printf("%s:\t", DBG);
	vprintf(fmt, args);
	va_end (args);
}

void print_error(const char *fmt, ...) {
	va_list args;
	va_start (args, fmt);
	fprintf(stderr, RED "%s:\t", ERROR);
	vprintf(fmt, args);
	fprintf(stderr, RESET);
	va_end (args);
}

void print_warning(const char *fmt, ...) {
	va_list args;
	va_start (args, fmt);
	fprintf(stderr, YEL "%s:\t", WARNING);
	vprintf(fmt, args);
	fprintf(stderr, RESET);
	va_end (args);
}

void print_success_status() {
	printf("\t" GRN "ok" RESET "\n");
}

void print_failure_status() {
	printf("\t" RED "failed" RESET "\n");
}
