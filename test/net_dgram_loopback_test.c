#include <stdio.h>
#include <stdlib.h>
#include <strings.h>	// For bzero
#include <fcntl.h>		// For fcntl
#include <unistd.h>		// For pipe
#include <stddef.h>		// For NULL
#include <sys/epoll.h>	// For EPOLLIN

#include "dx_debug_assert.h"
#include "dx_debug_malloc.h"

#include "dx_util_buffer.h"

#include "dx_event_mplexer.h"
#include "dx_net_packet.h"
#include "dx_net_dgram.h"

int dx_net_dgram_handler(dx_event_context_t* context, dx_packet_t* packet, struct sockaddr_in* peer_addr);

#define TEST_DGRAM_PORT 2017

int dx_send_hearbeat_broadcast(int fd, int to_port);

void net_dgram_loopback_test() {
	dx_dgram_context_t* pdgram;
	int i = 0;

	/* event multiplexer start */
	dx_event_mplexer_create();

	/* datagram server start */
	pdgram = dx_dgram_create();

	dx_dgram_set_service_port(pdgram, TEST_DGRAM_PORT);
	dx_dgram_start(pdgram, dx_net_dgram_handler);

	/* Big Loop */
	while(i++ < 1000) {
		dx_event_mplexer_poll(1000);

		if(i == 1) {
			dx_send_hearbeat_broadcast(pdgram->socket_fd, TEST_DGRAM_PORT);
		}
	}

	dx_dgram_destroy(pdgram);

	dx_event_mplexer_destroy();

	CHKMEM();
}

int dx_send_hearbeat_broadcast(int fd, int to_port) {
	dx_packet_hb_t* packet;
	uint32_t	len;

	len = DX_PACKET_HB_SIZE;

	packet = (dx_packet_hb_t*)MALLOC(len);
	packet->header.len = htonl(len);
	packet->header.type = DX_PACKET_TYPE_DISCOVERY;
	packet->header.code = 0;
	packet->header.data_type = DX_DATA_TYPE_NONE;

	dx_send_broadcast(fd, packet, to_port);

	FREE(packet);

	return 0;
}

int dx_net_dgram_handler(dx_event_context_t* context, dx_packet_t* packet, struct sockaddr_in* peer_addr) {
	switch(packet->header.type) {
	case DX_PACKET_TYPE_DISCOVERY : /* Discovery */

		printf("[DGRAM] Receive DISCOVERY....\n");
		dx_send_hearbeat_broadcast(context->fd, ntohs(peer_addr->sin_port));

		break;
	default:	/* Should not reach to here */
		ASSERT("Datagrem Event Handling.. should not reach to here.", !!0);
		break;
	}
    return 0;
}
