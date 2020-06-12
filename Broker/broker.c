/*
 ============================================================================
 Name        : broker.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "broker.h"
#include<commons/process.h>

void signal_handler(int sig){
	dump_cache();
}

int main(void) {

	signal(SIGUSR1, &signal_handler);
	unsigned int pid = process_getpid();
	printf("ID del proceso: %d\n", pid);

	initialization();
	behavior();
	termination();
	return EXIT_SUCCESS;
}
