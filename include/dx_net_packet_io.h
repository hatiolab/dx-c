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

#ifndef __DX_NET_PACKET_IO_H
#define __DX_NET_PACKET_IO_H

#include "dx_event_mplexer.h"
#include "dx_net_packet.h"

/*
 * Definitions
 */

/* APIs */

int dx_write(int fd, void* buf, ssize_t sz, int discardable);

int dx_read_with_block_mode(int fd, void* buf, ssize_t sz);
int dx_write_by_poller(dx_event_context_t* pcontext);
int dx_receive_packet(dx_event_context_t* pcontext, dx_packet_t** ppacket);
int dx_receive_dgram(dx_event_context_t* pcontext, dx_packet_t** ppacket, struct sockaddr_in* peer_addr);

#endif /* DX_NET_PACKET_IO_H */
