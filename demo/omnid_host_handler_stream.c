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

int od_host_handler_stream_playback(int fd, dx_packet_t* packet);

int od_host_handler_stream(int fd, dx_packet_t* packet) {

    switch(packet->header.code) {

	case DX_STREAM : 	/* 스트림 */
		break;
	case DX_STREAM_PLAYBACK : /* 플레이백 스트림 - 플레이타임 등 정보가 포함됨. */
		od_host_handler_stream_playback(fd, packet);
		break;
    }

	return 0;
}

/* For Playback Streaming */

time_t demo_stream_first_received_time = 0;
time_t demo_stream_last_received_time = 0;
int demo_stream_frames_received = 0;
unsigned long long demo_stream_first_ts = 0;

int od_host_handler_stream_playback(int fd, dx_packet_t* packet) {
	dx_stream_packet_t* stream = (dx_stream_packet_t*)packet;
	int framerate = 0;
	time_t now = time(NULL);
	long long elapsed = 0;

	if(now - demo_stream_last_received_time > 1) {
		/* reset all */
		demo_stream_first_received_time = now;
		demo_stream_last_received_time = now;
		demo_stream_frames_received = 1;
		demo_stream_first_ts = htonll(stream->data.ts);
	} else {
		demo_stream_frames_received++;
		/* 스트리밍 구간의 평균 프레임레이트를 계산함 */
		if(demo_stream_frames_received != 0 && now != demo_stream_first_received_time)
			framerate = (demo_stream_frames_received) / (now - demo_stream_first_received_time);
		demo_stream_last_received_time = now;
		elapsed = htonll(stream->data.ts) - demo_stream_first_ts;
	}

	LOG("Stream Received : %d, framerate %d, size (%d).",
			demo_stream_frames_received, framerate, htonl(stream->data.len));
	LOG("[Seq. %d, Flags %d, TS %lld (%lld).",
			htonl(stream->data.sequence), htons(stream->data.flags), htonll(stream->data.ts), elapsed);
	return 0;
}

