// Copyright (c) 2015 - 2015 All Right Reserved, http://imagenext.co.kr
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

#include "omnid.h"

int od_drive_handler_hb(int fd, dx_packet_t* packet) {
	printf("(Drive Event Handling) HeartBeat\n");

	dx_packet_send_heartbeat(fd, 0);

	printf("(Drive Event Handling) HeartBeat.. Sent back.\n");

	return 0;
}
