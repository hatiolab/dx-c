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

#ifndef __DX_DISCOVERY_H
#define __DX_DISCOVERY_H

#include "dx_net_dgram.h"

/*
 * Definitions
 */

dx_dgram_context_t* dx_discovery_server_start();
dx_dgram_context_t* dx_discovery_client_start();

#endif /* __DX_DISCOVERY_H */
