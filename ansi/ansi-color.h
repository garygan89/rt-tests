#ifndef ANSI_COLOR_H_
#define ANSI_COLOR_H_

/**
 * printf ANSI Color code for *nix terminal
 * Usage: printf(RED "red\n" RESET);
 * printf("This is " RED "red" RESET " and this is " BLU "blue" RESET "\n");
 */
#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"
#define BLU   "\x1B[34m"
#define MAG   "\x1B[35m"
#define CYN   "\x1B[36m"
#define WHT   "\x1B[37m"
#define RESET "\x1B[0m"

#endif
