// Copyright (c) 2015 - 2015 All Right Reserved, http://hatiolab.com
//
// This source is subject to the ImageNext Permissive License.
// Please see the License.txt file for more information.
// All other rights reserved.
//
// THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WIHOUT WARRANTY OF ANY
// KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//

#include <stdint.h>			// For uint8_t
#include <stdio.h>
#include <unistd.h>			// For read
#include <sys/socket.h>
#include <sys/epoll.h>		// For epoll
#include <arpa/inet.h>		// For inet_ntoa

#include "dx.h"
#include "dx_net_discovery.h"
#include "dx_event_mplexer.h"

#include "dx_net_client.h"
#include "dx_net_packet.h"
#include "dx_net_packet_file.h"
#include "dx_net_dgram.h"

#define DISCOVERY_BUFFER_LENGTH DX_PRIMITIVE_PACKET_SIZE

uint8_t __discovery_buffer[DISCOVERY_BUFFER_LENGTH];

int dx_discovery_server_handler(dx_event_context_t* context);
int dx_discovery_client_handler(dx_event_context_t* context);

dx_dgram_context_t* dx_discovery_server_start(int port) {
	dx_dgram_context_t* odc = dx_dgram_create();

	dx_dgram_set_service_port(odc, port);
	dx_dgram_listen(odc);

	dx_add_event_context(dx_dgram_get_fd(odc), EPOLLIN, dx_discovery_server_handler, NULL, NULL);

	return odc;
}

dx_dgram_context_t* dx_discovery_client_start(int port) {
	dx_dgram_context_t* odc = dx_dgram_create();

	dx_dgram_set_service_port(odc, port);
	dx_dgram_listen(odc);

	dx_add_event_context(dx_dgram_get_fd(odc), EPOLLIN, dx_discovery_client_handler, NULL, NULL);

	return odc;
}

int dx_discovery_server_handler(dx_event_context_t* context) {

	struct sockaddr_in client_addr;
	int ret, slen = sizeof(client_addr);
	dx_packet_t* packet = NULL;
	char hostname[INET_ADDRSTRLEN];

	int client_discovery_port = 0;

	ret = recvfrom(context->fd, __discovery_buffer, DISCOVERY_BUFFER_LENGTH, 0, (struct sockaddr*)&client_addr, &slen);
	if(ret == -1) {
		perror("Discovery Server recvfrom() error");
		close(context->fd);
		dx_del_event_context(context);
		return -1;
	}

	packet = (dx_packet_t*)__discovery_buffer;

	switch(packet->header.data_type) {

	case DX_DATA_TYPE_PRIMITIVE:

		client_discovery_port = ntohl(((dx_primitive_packet_t*)packet)->data.s32);

		((dx_primitive_packet_t*)packet)->data.s32 = htonl(dx_server_get_service_port());

		client_addr.sin_port = htons(client_discovery_port);

		packet->header.type = DX_PACKET_TYPE_DISCOVERY;
		packet->header.code = DX_DISCOVERY_RESP;

		ret = sendto(context->fd, packet, ntohl(packet->header.len), 0, (struct sockaddr*)&client_addr, slen);
		if(ret == -1) {
			perror("Discovery Server sendto() error");
			close(context->fd);
			dx_del_event_context(context);
			return -1;
		}

		break;

	default:
		printf("(Discovery Event Handling) Unknown(from %s)\n", inet_ntoa(client_addr.sin_addr));
		break;
	}

    return 0;
}

int dx_discovery_client_handler(dx_event_context_t* context) {

	struct sockaddr_in server_addr;
	int ret, slen = sizeof(server_addr);
	dx_packet_t* packet = NULL;

	int server_port = 0;

	ret = recvfrom(context->fd, __discovery_buffer, DISCOVERY_BUFFER_LENGTH, 0, (struct sockaddr*)&server_addr, &slen);
	if(ret == -1) {
		perror("Discovery Server recvfrom() error");
		close(context->fd);
		dx_del_event_context(context);
		return -1;
	}

	packet = (dx_packet_t*)__discovery_buffer;

	switch(packet->header.data_type) {

	case DX_DATA_TYPE_PRIMITIVE:

		server_port = ntohl(((dx_primitive_packet_t*)packet)->data.u32);

		printf("(Discovery Event Handling) Discovery(%d)\n", server_port);

		/*
		 * Server에 아직 연결되지 않았다면, 서버 연결을 시도한다.
		 */
		dx_host_start(inet_ntoa(server_addr.sin_addr), server_port);

		break;

	default:
		printf("(Discovery Event Handling) Unknown(from %s)\n", inet_ntoa(server_addr.sin_addr));
		break;
	}

    return 0;
}

int dx_discovery_send_broadcast(dx_dgram_context_t* odc, int port) {
	struct sockaddr_in	broadcast_addr;
	dx_primitive_packet_t* packet;
	uint32_t	len, ret;

	len = DX_PRIMITIVE_PACKET_SIZE;

	packet = (dx_primitive_packet_t*)__discovery_buffer;
	packet->header.len = htonl(len);
	packet->header.type = DX_PACKET_TYPE_DISCOVERY;
	packet->header.code = DX_DISCOVERY_REQ;
	packet->header.data_type = DX_DATA_TYPE_PRIMITIVE;
	packet->data.s32 = htonl(dx_dgram_get_service_port(odc));

	/* Send to broadcast */

	broadcast_addr.sin_family = AF_INET;
	broadcast_addr.sin_addr.s_addr = htonl(INADDR_BROADCAST);
	broadcast_addr.sin_port = htons(port);
	memset(&(broadcast_addr.sin_zero), '\0', 8);

	ret = sendto(odc->socket_fd, __discovery_buffer, len, 0, (struct sockaddr*)&broadcast_addr, sizeof(broadcast_addr));
	if(ret == -1) {
		perror("dx_discovery_send_broadcast");
	}

	return 0;
}
