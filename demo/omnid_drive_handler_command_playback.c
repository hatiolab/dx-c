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

extern dx_buffer_t* dx_frame_buffer;

int demo_playback_video_file = -1;
dx_schedule_t* demo_playback_stream_schedule = NULL;

int demo_playback_frame_idx = 0;

void demo_playback_schedule_callback(void* sender_fd) {
	dx_buffer_t* frame;

	if(dx_frame_buffer == NULL)
		dx_frame_buffer = dx_buffer_alloc(DX_H264_FRAME_MAX_SIZE);

	frame = dx_h264_read_frame(demo_playback_video_file, dx_frame_buffer, demo_playback_frame_idx);
	if(frame != NULL) {
		dx_packet_send_stream((int)sender_fd, DX_STREAM, 0 /* enctype */, dx_buffer_ppos(frame), dx_buffer_remains(frame));
	} else {
		demo_playback_frame_idx = 0;
		dx_schedule_cancel(demo_playback_stream_schedule);
	}

	demo_playback_frame_idx++;
}

void od_on_playback_start(int fd) {
	/* 만약 현재 동작중인 스케쥴러가 있으면, 동작하지 않음 */
	if(demo_playback_stream_schedule != NULL && demo_playback_stream_schedule->next_schedule != 0) {
		fprintf(stderr, "이미 동작중인 스트리밍 스케쥴러가 있습니다.");
		return;
	}

	if(demo_playback_video_file >= 0) {
		close(demo_playback_video_file);
		printf("현재 열려있는 비디오 파일을 닫았습니다.\n");
		demo_playback_video_file = -1;
	}

	demo_playback_video_file = dx_h264_open("/home/in/1.avi");
	if(demo_playback_video_file == -1) {
		perror("비디오파일 열기를 실패하였습니다.");
		return;
	}

	/* 새로운 스트리밍 스케쥴러를 등록하고, 바로 시작합니다. */
	demo_playback_stream_schedule = dx_schedule_register(0, 1000/30 /* 30 frames */, 1, demo_playback_schedule_callback, (void*)fd);
	dx_event_mplexer_wakeup();
}

void od_on_playback_stop(int fd) {
	if(demo_playback_stream_schedule != NULL && demo_playback_stream_schedule->next_schedule != 0) {
		/* 스케쥴러를 취소합니다. */
		dx_schedule_cancel(demo_playback_stream_schedule);
		demo_playback_stream_schedule = NULL;
	}

	if(demo_playback_video_file >= 0) {
		close(demo_playback_video_file);
		demo_playback_video_file = -1;
	}
}
