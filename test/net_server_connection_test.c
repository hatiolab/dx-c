#include <stdio.h>
#include <stdlib.h>

#include "dx.h"

#include "dx_debug_assert.h"
#include "dx_debug_malloc.h"

#include "dx_util_buffer.h"
#include "dx_util_log.h"
#include "dx_util_schedule.h"

#include "dx_event_mplexer.h"
#include "dx_net_server.h"
#include "dx_net_client.h"

#define TEST_SERVICE_PORT 2018

int connection_test_schedule_callback(void* p);
int dx_net_server_connection_test_handler(dx_event_context_t* context, dx_packet_t* packet);
int dx_net_client_connection_test_handler(dx_event_context_t* context, dx_packet_t* packet);

int connection_test_dx_client;
int connection_test_dx_server;

int connection_test_disconnect_event_called = 0;
int connection_test_server_connect_event_called = 0;
int connection_test_client_connect_event_called = 0;

void net_server_connection_test() {

	int connection_test_schedule_count = 0;

	dx_event_mplexer_create();

	dx_server_start(TEST_SERVICE_PORT, dx_net_server_connection_test_handler);
	dx_client_start("localhost", TEST_SERVICE_PORT, dx_net_client_connection_test_handler);

	dx_scheduler_start();

	dx_schedule_register(100, 10, 1/* repeatable */, connection_test_schedule_callback, (void*)&connection_test_schedule_count);

	/* Big Loop */
	while(1) {
		int wait = dx_scheduler_next_wait();
		dx_event_mplexer_poll(wait);

		dx_scheduler_do();

		if(connection_test_schedule_count % 10 == 0)
			close(connection_test_dx_client);
		if(connection_test_schedule_count > 20)
			break;
	}

	CONSOLE("Ready to stop\n");

	dx_scheduler_stop();

	dx_event_mplexer_destroy();

	ASSERT("Connect Event Should be issued to Server\n", connection_test_server_connect_event_called == 1);
	ASSERT("Connect Event Should be issued to Client\n", connection_test_client_connect_event_called == 1);
	ASSERT("Disconnect Event Should be issued\n", connection_test_disconnect_event_called == 1);

	CHKMEM();
}

int connection_test_schedule_callback(void* p) {
	int *count = (int*)p;

	*count += 1;

	return 0;
}

int dx_net_server_connection_test_handler(dx_event_context_t* context, dx_packet_t* packet) {

	if(packet->header.type == DX_PACKET_TYPE_EVENT) {
		if(packet->header.code == DX_EVT_DISCONNECT) {
			connection_test_disconnect_event_called++;
		} else if(packet->header.code == DX_EVT_CONNECT) {
			connection_test_dx_server = context->fd;
			connection_test_server_connect_event_called++;
		}
	} else if(packet->header.type == DX_PACKET_TYPE_HB) {
		close(context->fd);
	}

	return 0;
}

int dx_net_client_connection_test_handler(dx_event_context_t* context, dx_packet_t* packet) {

	if(packet->header.type == DX_PACKET_TYPE_EVENT) {
		if(packet->header.code == DX_EVT_DISCONNECT) {

		} else if(packet->header.code == DX_EVT_CONNECT) {
			connection_test_dx_client = context->fd;
			connection_test_client_connect_event_called++;
		}
	} else if(packet->header.type == DX_PACKET_TYPE_HB) {
		close(context->fd);
	}

	return 0;
}
