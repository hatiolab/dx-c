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

#ifndef __DX_DISCOVERY_H
#define __DX_DISCOVERY_H

#include "dx_net_dgram.h"

/*
 * Definitions
 */

/*
 * discovery 클라이언트가 연결할 서버 정보를 찾아냈을 때, 사용자기능 쪽으로 정보를 알려주기 위한 콜백함수
 */
typedef void (*dx_discovery_found_callback)(char* hostname, int port);
/*
 * discovery 클라이언트에 알려줄 서비스 포트 정보를 사용자 기능 쪽에서 discovery server로 알려주는 콜백함수
 */
typedef void (*dx_discovery_server_callback)(int* port);

int dx_discovery_server_start(int fd, dx_discovery_server_callback callback);
int dx_discovery_client_start(int fd, dx_discovery_found_callback callback);

int dx_discovery_send_broadcast(int fd, int port);

#endif /* __DX_DISCOVERY_H */
