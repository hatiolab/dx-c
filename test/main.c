#include <stdio.h>
#include <stdlib.h>

#include "dx_util_buffer.h"

void util_buffer_test();
void debug_malloc_test();
void util_lock_test();
void net_pipe_test();
void net_server_test();
void console_test();
void util_list_test();
void net_discovery_test();

int main(void) {

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

//	printf("\nTest - net_pipe_test()..\n");
//	net_pipe_test();
//	printf("..Done\n");

//	printf("\nTest - event_console_test()..\n");
//	console_test();
//	printf("..Done\n");

	printf("\nTest - net_server_test()..\n");
	net_server_test();
	printf("..Done\n");

	printf("\nTest - net_dgram_loopback_test()..\n");
	net_dgram_loopback_test();
	printf("..Done\n");

	printf("\nTest - net_discovery_test()..\n");
	net_discovery_test();
	printf("..Done\n");

	return EXIT_SUCCESS;
}
