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
#include "dx_net.h"

/*
 * Definitions
 */

typedef struct dx_dgram_context {

	uint16_t	service_port;
	int 		socket_fd;
} dx_dgram_context_t;

/* APIs */

int dx_dgram_set_service_port(dx_dgram_context_t* odc, uint16_t port);
int dx_dgram_get_service_port(dx_dgram_context_t* odc);
int dx_dgram_get_fd(dx_dgram_context_t* odc);
int dx_dgram_listen(dx_dgram_context_t* odc);
dx_dgram_context_t* dx_dgram_create();
int dx_dgram_destroy(dx_dgram_context_t* odc);

#endif /* DX_NET_DGRAM_H */
