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

dx_movie_context_t* demo_movie_context = NULL;
dx_schedule_t* demo_playback_stream_schedule = NULL;

int demo_playback_fragment_idx = 0;
int demo_playback_stream_idx = 0;

int8_t* demo_playback_buffer = NULL;

int demo_playback_schedule_callback(void* sender_fd) {
	dx_movie_context_t* context = demo_movie_context;
	off_t seek_pos;

	while(context->total_fragment > demo_playback_fragment_idx) {
		/* 여기서 00dc를 찾는다. */
		dx_avi_index_entry_t entry;

		seek_pos = context->index_offset + sizeof(dx_avi_index_entry_t) * demo_playback_fragment_idx;
		seek_pos = lseek(context->fd, seek_pos, SEEK_SET);

		read(context->fd, &entry, sizeof(entry));
		if(strncmp("00dc", (char*)&entry.ckid, 4) != 0) {
			demo_playback_fragment_idx++;
			continue;
		}

		LOG("%05d - INDEX %-6.4s (%d bytes from %d)[flag:%d]",
				demo_playback_fragment_idx, entry.ckid, entry.length, entry.offset, entry.flags);

		/*
		 * TODO 여기서 프레임버퍼로
		 */
		if(demo_playback_buffer == NULL)
			demo_playback_buffer = MALLOC(3 * 1024 * 1024);

		lseek(context->fd, context->frame_offset + entry.offset, SEEK_SET);
		read(context->fd, demo_playback_buffer, entry.length);

		if(-1 == dx_packet_send_stream((int)sender_fd, DX_STREAM_PLAYBACK, 0 /* enctype */,
			entry.flags & 0x10 ? 1 : 0, demo_playback_stream_idx++, demo_playback_buffer, entry.length)) {
			/* Network Error */
			break;
		}

		demo_playback_fragment_idx++;
		return 0;
	}

	demo_playback_fragment_idx = 0;
	demo_playback_stream_idx = 0;
	dx_schedule_cancel(demo_playback_stream_schedule);

	if(demo_playback_buffer != NULL) {
		FREE(demo_playback_buffer);
		demo_playback_buffer = NULL;
	}

	CONSOLE("Playback Stream End.\n");
	return 0;
}

void od_on_playback_start(int fd, dx_packet_t* packet) {
	dx_u8_array_packet_t* strpacket = (dx_u8_array_packet_t*)packet;
	char path[256] = { 0 };

	/* 만약 현재 동작중인 스케쥴러가 있으면, 동작하지 않음 */
	if(demo_playback_stream_schedule != NULL && demo_playback_stream_schedule->next_schedule != 0) {
		ERROR("이미 동작중인 스트리밍 스케쥴러가 있습니다.");
		return;
	}

	if(demo_movie_context != NULL) {
		close(demo_movie_context->fd);
		CONSOLE("현재 열려있는 비디오 파일을 닫았습니다.\n");
		FREE(demo_movie_context);
		demo_movie_context = NULL;
	}

	demo_playback_fragment_idx = 0;
	demo_playback_stream_idx = 0;

	bzero(path, 256);
	memcpy(path, strpacket->array.data, ntohl(strpacket->array.len));

	demo_movie_context = dx_movie_context_create(path);

	if(demo_movie_context == NULL) {
		ERROR("동영상 파일 오픈에 실패하였습니다.");
		return;
	}

	CONSOLE("\nFrame Rate : %d\n", demo_movie_context->framerate);
	CONSOLE("Total Frames : %d\n", demo_movie_context->total_frame);
	CONSOLE("Total Fragments : %d\n", demo_movie_context->total_fragment);
	CONSOLE("Play Time : %d\n", demo_movie_context->playtime);
	CONSOLE("Track Count : %d\n", demo_movie_context->track_count);

	CONSOLE("Frame Offset : %ld\n", demo_movie_context->frame_offset);
	CONSOLE("Index Offset : %ld\n", demo_movie_context->index_offset);

	CONSOLE("Width : %d\n", demo_movie_context->width);
	CONSOLE("Height : %d\n", demo_movie_context->height);

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

	if(demo_movie_context != NULL) {
		close(demo_movie_context->fd);
		FREE(demo_movie_context);
		demo_movie_context = NULL;
	}

	demo_playback_fragment_idx = 0;
	demo_playback_stream_idx = 0;
}
