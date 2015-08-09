// Copyright (c) 2015 - 2015 All Right Reserved, http://hatiolab.com
//
// This source is subject to the Hatio, Lab. Permissive License.
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

#include <netinet/in.h>   // For uint16_t

#include "dx_event_mplexer.h"
#include "dx_net_packet.h"
#include "dx_net.h"

/*
 * Definitions
 */

#define DX_DGRAM_MAX_PACKET_SIZE  1024

typedef void (*dx_dgram_event_handler)(dx_event_context_t* pcontext, dx_packet_t* ppacket, struct sockaddr_in* peer_addr);

/* APIs */

int dx_dgram_create();
int dx_dgram_get_service_port(int fd);
int dx_dgram_listen(int fd, int port);

/*
 * dx_dgram_start
 *
 * 아래 작업을 한번에 제공한다.
 * dx_dgram_create() => dx_dgram_listen() => register to event multiplexer
 */
int dx_dgram_start(int port, dx_dgram_event_handler handler);

#endif /* DX_NET_DGRAM_H */
