#include <stdio.h>
#include <stdlib.h>

#include "dx_util_buffer.h"

void debug_malloc_test();

void util_buffer_test();
void util_lock_test();
void util_list_test();
void util_clock_test();
void util_schedule_test();

void net_pipe_test();
void net_server_test();
void net_dgram_loopback_test();
void net_discovery_test();

void event_control_test();
void console_test();
void file_avi_test();

int main(void) {

	printf("\nTest - dx_avi_test()..\n");
	file_avi_test();
	printf("..Done\n");

	printf("\nTest - debug_malloc_test()..\n");
	debug_malloc_test();
	printf("..Done\n");

	printf("\nTest - util_buffer_test()..\n");
	util_buffer_test();
	printf("..Done\n");

	printf("\nTest - util_lock_test()..\n");
	util_lock_test();
	printf("..Done\n");

	printf("\nTest - util_list_test()..\n");
	util_list_test();
	printf("..Done\n");

	printf("\nTest - util_clock_test()..\n");
	util_clock_test();
	printf("..Done\n");

	printf("\nTest - util_schedule_test()..\n");
	util_schedule_test();
	printf("..Done\n");

	printf("\nTest - net_pipe_test()..\n");
	net_pipe_test();
	printf("..Done\n");

	printf("\nTest - net_server_test()..\n");
	net_server_test();
	printf("..Done\n");

	printf("\nTest - net_dgram_loopback_test()..\n");
	net_dgram_loopback_test();
	printf("..Done\n");

	printf("\nTest - net_discovery_test()..\n");
	net_discovery_test();
	printf("..Done\n");

	printf("\nTest - event_control_test()..\n");
	event_control_test();
	printf("..Done\n");

//	printf("\nTest - event_console_test()..\n");
//	console_test();
//	printf("..Done\n");

	fflush(stdout);

	return EXIT_SUCCESS;
}
