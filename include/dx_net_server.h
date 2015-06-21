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

#ifndef __DX_NET_SERVER_H
#define __DX_NET_SERVER_H

#include <netinet/in.h>		// For uint16_t
#include "dx_net.h"

/*
 * Definitions
 */

/* APIs */

int dx_server_create();
int dx_server_listen();
int dx_server_destroy();
int dx_server_accept_client();
int dx_server_get_service_port();
int	dx_server_set_service_port(uint16_t port);
int dx_server_get_fd();

int dx_server_acceptable_handler(dx_event_context_t* context);
int dx_server_writable_handler(dx_event_context_t* context);
int dx_server_readable_handler(dx_event_context_t* context);
int dx_server_start(int port);

#endif /* DX_NET_SERVER_H */
