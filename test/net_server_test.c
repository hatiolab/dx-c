#include <stdio.h>
#include <stdlib.h>
#include <strings.h>	// For bzero
#include <fcntl.h>		// For fcntl
#include <unistd.h>		// For server
#include <stddef.h>		// For NULL
#include <sys/epoll.h>	// For EPOLLIN

#include "dx_debug_assert.h"
#include "dx_debug_malloc.h"

#include "dx_util_buffer.h"

#include "dx_event_mplexer.h"
#include "dx_net_server.h"
#include "dx_net_client.h"

#include "dx.h"

int dx_net_server_handler(dx_event_context_t* context, dx_packet_t* packet);
int dx_net_client_handler(dx_event_context_t* context, dx_packet_t* packet);

int dx_server_handler_hb(dx_event_context_t* context, dx_packet_t* packet);
int dx_server_handler_get_setting(dx_event_context_t* context, dx_packet_t* packet);
int dx_server_handler_set_setting(dx_event_context_t* context, dx_packet_t* packet);
int dx_server_handler_get_state(dx_event_context_t* context, dx_packet_t* packet);
int dx_server_handler_set_state(dx_event_context_t* context, dx_packet_t* packet);
int dx_server_handler_event(dx_event_context_t* context, dx_packet_t* packet);
int dx_server_handler_command(dx_event_context_t* context, dx_packet_t* packet);
int dx_server_handler_file(dx_event_context_t* context, dx_packet_t* packet);

int dx_client_handler_hb(dx_event_context_t* context, dx_packet_t* packet);
int dx_client_handler_get_setting(dx_event_context_t* context, dx_packet_t* packet);
int dx_client_handler_set_setting(dx_event_context_t* context, dx_packet_t* packet);
int dx_client_handler_get_state(dx_event_context_t* context, dx_packet_t* packet);
int dx_client_handler_set_state(dx_event_context_t* context, dx_packet_t* packet);
int dx_client_handler_event(dx_event_context_t* context, dx_packet_t* packet);
int dx_client_handler_command(dx_event_context_t* context, dx_packet_t* packet);
int dx_client_handler_file(dx_event_context_t* context, dx_packet_t* packet);

#define TEST_SERVICE_PORT 2017

int dx_client;
int dx_server;

void net_server_test() {
	int i = 0;

	dx_event_mplexer_create();

	dx_server = dx_server_start(TEST_SERVICE_PORT, dx_net_server_handler);
	dx_client = dx_client_start("localhost", TEST_SERVICE_PORT, dx_net_client_handler);

	/* Big Loop */
	while(i++ < 1000) {
		dx_event_mplexer_poll(10000);

		if(i == 1) {
			dx_packet_send_heartbeat(dx_client, 0);
		}
	}

	dx_event_mplexer_destroy();

	CHKMEM();
}

int dx_net_server_handler(dx_event_context_t* context, dx_packet_t* packet) {
	switch(packet->header.type) {
	case DX_PACKET_TYPE_HB : /* Heart Beat */
		dx_server_handler_hb(context, packet);
		break;
	case DX_PACKET_TYPE_GET_SETTING	: /* Get Setting */
		dx_server_handler_get_setting(context, packet);
		break;
	case DX_PACKET_TYPE_SET_SETTING : /* Set Setting */
		dx_server_handler_set_setting(context, packet);
		break;
	case DX_PACKET_TYPE_GET_STATE : /* Get State */
		dx_server_handler_get_state(context, packet);
		break;
	case DX_PACKET_TYPE_SET_STATE : /* Set State */
		dx_server_handler_set_state(context, packet);
		break;
	case DX_PACKET_TYPE_EVENT : /* Event */
		dx_server_handler_event(context, packet);
		break;
	case DX_PACKET_TYPE_COMMAND : /* Command */
		dx_server_handler_command(context, packet);
		break;
	case DX_PACKET_TYPE_FILE 	: /* File */
		dx_server_handler_file(context, packet);
		break;
	default:	/* Should not reach to here */
		ASSERT("Server Event Handling.. should not reach to here.", !!0);
		break;
	}
    return 0;
}

int dx_server_handler_hb(dx_event_context_t* context, dx_packet_t* packet) {
	printf("[Server] Receive HB....\n");

	dx_packet_send_heartbeat(context->fd, 0);

	return 0;
}

int dx_server_handler_get_setting(dx_event_context_t* context, dx_packet_t* packet) {
	return 0;
}

int dx_server_handler_set_setting(dx_event_context_t* context, dx_packet_t* packet) {
	return 0;
}

int dx_server_handler_get_state(dx_event_context_t* context, dx_packet_t* packet) {
	return 0;
}

int dx_server_handler_set_state(dx_event_context_t* context, dx_packet_t* packet) {
	return 0;
}

int dx_server_handler_event(dx_event_context_t* context, dx_packet_t* packet) {
	return 0;
}

int dx_server_handler_command(dx_event_context_t* context, dx_packet_t* packet) {
	return 0;
}

int dx_server_handler_file(dx_event_context_t* context, dx_packet_t* packet) {
	return 0;
}

int dx_net_client_handler(dx_event_context_t* context, dx_packet_t* packet) {
	switch(packet->header.type) {
	case DX_PACKET_TYPE_HB : /* Heart Beat */
		dx_client_handler_hb(context, packet);
		break;
	case DX_PACKET_TYPE_GET_SETTING	: /* Get Setting */
		dx_client_handler_get_setting(context, packet);
		break;
	case DX_PACKET_TYPE_SET_SETTING : /* Set Setting */
		dx_client_handler_set_setting(context, packet);
		break;
	case DX_PACKET_TYPE_GET_STATE : /* Get State */
		dx_client_handler_get_state(context, packet);
		break;
	case DX_PACKET_TYPE_SET_STATE : /* Set State */
		dx_client_handler_set_state(context, packet);
		break;
	case DX_PACKET_TYPE_EVENT : /* Event */
		dx_client_handler_event(context, packet);
		break;
	case DX_PACKET_TYPE_COMMAND : /* Command */
		dx_client_handler_command(context, packet);
		break;
	case DX_PACKET_TYPE_FILE 	: /* File */
		dx_client_handler_file(context, packet);
		break;
	default:	/* Should not reach to here */
		ASSERT("Client Event Handling.. should not reach to here.", !!0);
		break;
	}
    return 0;
}

int dx_client_handler_hb(dx_event_context_t* context, dx_packet_t* packet) {
	printf("[Client] Receive HB....\n");

	dx_packet_send_heartbeat(context->fd, 0);

	return 0;
}

int dx_client_handler_get_setting(dx_event_context_t* context, dx_packet_t* packet) {
	return 0;
}

int dx_client_handler_set_setting(dx_event_context_t* context, dx_packet_t* packet) {
	return 0;
}

int dx_client_handler_get_state(dx_event_context_t* context, dx_packet_t* packet) {
	return 0;
}

int dx_client_handler_set_state(dx_event_context_t* context, dx_packet_t* packet) {
	return 0;
}

int dx_client_handler_event(dx_event_context_t* context, dx_packet_t* packet) {
	return 0;
}

int dx_client_handler_command(dx_event_context_t* context, dx_packet_t* packet) {
	return 0;
}

int dx_client_handler_file(dx_event_context_t* context, dx_packet_t* packet) {
	return 0;
}

