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

#ifndef __DX_NET_CLIENT_H
#define __DX_NET_CLIENT_H

#include <netinet/in.h>   // For uint16_t

#include "dx_event_mplexer.h"

#include "dx_net.h"
#include "dx_net_packet.h"

/*
 * Definitions
 */
typedef int (*dx_client_event_handler)(dx_event_context_t* pcontext, dx_packet_t* packet);

/* APIs */
int dx_client_connect(char* hostname, uint16_t port);

int dx_client_writable_handler(dx_event_context_t* context);
int dx_client_readable_handler(dx_event_context_t* context);
int dx_client_start(char* hostname, int port, dx_client_event_handler handler);

#endif /* DX_NET_CLIENT_H */
