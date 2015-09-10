#include <stdio.h>
#include <stdlib.h>

#include "dx.h"

#include "dx_util_buffer.h"
#include "dx_util_log.h"

void debug_malloc_test();

void util_buffer_test();
void util_lock_test();
void util_list_test();
void util_clock_test();
void util_schedule_test();

void net_packet_file_test();
void net_pipe_test();
void net_server_test();
void net_dgram_loopback_test();
void net_discovery_test();
void net_server_connection_test();

void event_control_test();
void console_test();
void file_avi_test();


int main(void) {

	CONSOLE("\nTest - dx_packet_file_test()..\n");
	net_packet_file_test();
	CONSOLE("..Done\n");

	CONSOLE("\nTest - net_server_connection_test()..\n");
	net_server_connection_test();
	CONSOLE("..Done\n");

	CONSOLE("\nTest - dx_avi_test()..\n");
	file_avi_test();
	CONSOLE("..Done\n");

	CONSOLE("\nTest - debug_malloc_test()..\n");
	debug_malloc_test();
	CONSOLE("..Done\n");

	CONSOLE("\nTest - util_buffer_test()..\n");
	util_buffer_test();
	CONSOLE("..Done\n");

	CONSOLE("\nTest - util_lock_test()..\n");
	util_lock_test();
	CONSOLE("..Done\n");

	CONSOLE("\nTest - util_list_test()..\n");
	util_list_test();
	CONSOLE("..Done\n");

	CONSOLE("\nTest - util_list_thread_test()..\n");
	util_list_thread_test();
	CONSOLE("..Done\n");

	CONSOLE("\nTest - util_clock_test()..\n");
	util_clock_test();
	CONSOLE("..Done\n");

	CONSOLE("\nTest - util_schedule_test()..\n");
	util_schedule_test();
	CONSOLE("..Done\n");

	CONSOLE("\nTest - net_pipe_test()..\n");
	net_pipe_test();
	CONSOLE("..Done\n");

	CONSOLE("\nTest - net_server_test()..\n");
	net_server_test();
	CONSOLE("..Done\n");

	CONSOLE("\nTest - net_dgram_loopback_test()..\n");
	net_dgram_loopback_test();
	CONSOLE("..Done\n");

	CONSOLE("\nTest - net_discovery_test()..\n");
	net_discovery_test();
	CONSOLE("..Done\n");

	CONSOLE("\nTest - event_control_test()..\n");
	event_control_test();
	CONSOLE("..Done\n");

//	CONSOLE("\nTest - event_console_test()..\n");
//	console_test();
//	CONSOLE("..Done\n");

	fflush(stdout);

	return EXIT_SUCCESS;
}
