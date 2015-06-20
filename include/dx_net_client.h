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

#ifndef __DX_NET_CLIENT_H
#define __DX_NET_CLIENT_H

#include <netinet/in.h>		// For uint16_t
#include "dx_net.h"

/*
 * Definitions
 */

/* APIs */
int dx_client_create();
int	dx_client_connect(char* hostname, uint16_t port);
int dx_client_destroy();

int dx_client_get_fd();

#endif /* DX_NET_CLIENT_H */
