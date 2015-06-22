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

#ifndef __DX_NET_DGRAM_H
#define __DX_NET_DGRAM_H

#include <netinet/in.h>		// For uint16_t

#include "dx_event_mplexer.h"
#include "dx_net_packet.h"
#include "dx_net.h"

/*
 * Definitions
 */

#define DX_DGRAM_MAX_PACKET_SIZE	2048

typedef void (*dx_dgram_event_handler)(dx_event_context_t* pcontext, dx_packet_t* ppacket, struct sockaddr_in* peer_addr);

typedef struct dx_dgram_context {

	uint16_t	service_port;
	int 		socket_fd;
} dx_dgram_context_t;

/* APIs */

int dx_dgram_set_service_port(dx_dgram_context_t* ddc, uint16_t port);
int dx_dgram_get_service_port(dx_dgram_context_t* ddc);
int dx_dgram_get_fd(dx_dgram_context_t* ddc);
int dx_dgram_listen(dx_dgram_context_t* ddc);
dx_dgram_context_t* dx_dgram_create();
int dx_dgram_destroy(dx_dgram_context_t* ddc);

int dx_dgram_start(dx_dgram_context_t* ddc, dx_dgram_event_handler handler);

#endif /* DX_NET_DGRAM_H */
