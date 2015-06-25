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

time_t demo_stream_first_received_time = 0;
time_t demo_stream_last_received_time = 0;
int demo_stream_frames_received = 0;

int od_host_handler_stream(int fd, dx_packet_t* packet) {
	dx_stream_packet_t* stream = (dx_stream_packet_t*)packet;
	int framerate = 0;
	time_t now = time(NULL);

	if(now - demo_stream_last_received_time > 1) {
		/* reset all */
		demo_stream_first_received_time = now;
		demo_stream_last_received_time = now;
		demo_stream_frames_received++;
	} else {
		demo_stream_frames_received++;
		if(demo_stream_frames_received != 0 && now != demo_stream_first_received_time)
			framerate = (demo_stream_frames_received) / (now - demo_stream_first_received_time);
		demo_stream_last_received_time = now;
	}

	printf("Stream Received : %d, framerate %d, size (%d).\n", demo_stream_frames_received, framerate, htonl(stream->data.len));
}

