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

#ifndef __DX_NET_H
#define __DX_NET_H

#include <netinet/in.h>   // For uint16_t

/*
 * Definitions
 */

#define DX_DEFAULT_SERVICE_PORT 2015
#define DX_SOCKET_BUF_SIZE  (8192 * 4)
#define DX_DGRAM_BUF_SIZE  (8192 * 4)

#define ntohll(x) (((uint64_t)(ntohl((uint32_t)((x<<32)>>32))) << 32) | ntohl( ((uint32_t)(x >> 32))))
#define htonll(x) ntohll(x)

#endif /* DX_NET_H */
