#include <stdio.h>
#include <stdlib.h>
#include <strings.h>	// For bzero
#include <fcntl.h>		// For fcntl
#include <unistd.h>		// For pipe
#include <stddef.h>		// For NULL
#include <sys/epoll.h>	// For EPOLLIN

#include "dx.h"

#include "dx_debug_assert.h"
#include "dx_debug_malloc.h"

#include "dx_util_buffer.h"
#include "dx_util_log.h"

#include "dx_event_mplexer.h"
#include "dx_net_packet.h"
#include "dx_net_dgram.h"

void net_dgram_loopback_handler(dx_event_context_t* context, dx_packet_t* packet, struct sockaddr_in* peer_addr);

#define TEST_DGRAM_PORT 2017

int dx_send_hearbeat_broadcast(int fd, int to_port);

void net_dgram_loopback_test() {
	int fd;
	int i = 0;

	/* event multiplexer start */
	dx_event_mplexer_create();

	/* datagram server start */
	fd = dx_dgram_start(TEST_DGRAM_PORT, net_dgram_loopback_handler);

	/* Big Loop */
	while(i++ < 10) {
		dx_event_mplexer_poll(1000);

		if(i == 1) {
			dx_send_hearbeat_broadcast(fd, TEST_DGRAM_PORT);
		}
	}

	dx_event_mplexer_destroy();

	CHKMEM();
}

int dx_send_hearbeat_broadcast(int fd, int to_port) {
	dx_packet_hb_t* packet;
	uint32_t	len;

	len = DX_PACKET_HB_SIZE;

	packet = (dx_packet_hb_t*)MALLOC(len);
	packet->header.len = htonl(len);
	packet->header.type = DX_PACKET_TYPE_HB;
	packet->header.code = 0;
	packet->header.data_type = DX_DATA_TYPE_NONE;

	dx_send_broadcast(fd, (dx_packet_t*)packet, to_port);

	FREE(packet);

	return 0;
}

void net_dgram_loopback_handler(dx_event_context_t* context, dx_packet_t* packet, struct sockaddr_in* peer_addr) {
	switch(packet->header.type) {
	case DX_PACKET_TYPE_HB : /* Heartbeat */

		CONSOLE("[DGRAM] Receive Heartbeat....\n");
		CONSOLE("Port : %d, %d\n", peer_addr->sin_port, ntohs(peer_addr->sin_port));
		dx_send_hearbeat_broadcast(context->fd, ntohs(peer_addr->sin_port));

		break;
	default:	/* Should not reach to here */
		ASSERT("Datagrem Event Handling.. should not reach to here.", !!0);
		break;
	}
}
