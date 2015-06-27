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

int demo_playback_video_file = -1;
dx_schedule_t* demo_playback_stream_schedule = NULL;

int demo_playback_frame_idx = 0;

off_t demo_playback_video_movie_offset = -1;
AVI_LIST demo_playback_video_movie_list;

off_t demo_playback_video_index_offset = -1;
AVI_CHUNK demo_playback_video_index_chunk;

int8_t* demo_playback_buffer = NULL;

void demo_playback_schedule_callback(void* sender_fd) {
	dx_buffer_t* frame;
	off_t seek_pos;

	while(demo_playback_video_index_chunk.size > (sizeof(dx_avi_index_entry_t) * demo_playback_frame_idx)) {
		/* 여기서 00dc를 찾는다. */
		dx_avi_index_entry_t entry;

		seek_pos = demo_playback_video_index_offset + sizeof(dx_avi_index_entry_t) * demo_playback_frame_idx;
		seek_pos = lseek(demo_playback_video_file, seek_pos, SEEK_SET);

		read(demo_playback_video_file, &entry, sizeof(entry));
		if(strncmp("00dc", (char*)&entry.ckid, 4) != 0) {
			demo_playback_frame_idx++;
			continue;
		}

		LOG("%05d - ", demo_playback_frame_idx);
		dx_avi_index_print(&entry);

		/*
		 * TODO 여기서 프레임버퍼로
		 */
		if(demo_playback_buffer == NULL)
			demo_playback_buffer = MALLOC(3 * 1024 * 1024);

		lseek(demo_playback_video_file, demo_playback_video_movie_offset + entry.offset, SEEK_SET);
		read(demo_playback_video_file, demo_playback_buffer, entry.length);

		dx_packet_send_stream((int)sender_fd, DX_STREAM_PLAYBACK, 0 /* enctype */, demo_playback_buffer, entry.length);

		demo_playback_frame_idx++;
		return;
	}

	demo_playback_frame_idx = 0;
	dx_schedule_cancel(demo_playback_stream_schedule);

	if(demo_playback_buffer != NULL) {
		FREE(demo_playback_buffer);
		demo_playback_buffer = NULL;
	}

	CONSOLE("Playback Stream End.");
}

void od_on_playback_start(int fd, dx_packet_t* packet) {
	dx_u8_array_packet_t* strpacket = (dx_u8_array_packet_t*)packet;
	char path[256] = { NULL };

	/* 만약 현재 동작중인 스케쥴러가 있으면, 동작하지 않음 */
	if(demo_playback_stream_schedule != NULL && demo_playback_stream_schedule->next_schedule != 0) {
		ERROR("이미 동작중인 스트리밍 스케쥴러가 있습니다.");
		return;
	}

	if(demo_playback_video_file >= 0) {
		close(demo_playback_video_file);
		CONSOLE("현재 열려있는 비디오 파일을 닫았습니다.\n");
		demo_playback_video_file = -1;
	}

	bzero(path, 256);
	memcpy(path, strpacket->array.data, ntohl(strpacket->array.len));

	demo_playback_video_file = dx_avi_open(path);

	demo_playback_video_movie_offset = dx_avi_find_movie_list(demo_playback_video_file, &demo_playback_video_movie_list);
	demo_playback_video_movie_offset += sizeof(AVI_LIST);

	demo_playback_video_index_offset = dx_avi_find_index_chunk(demo_playback_video_file, &demo_playback_video_index_chunk);
	demo_playback_video_index_offset += sizeof(AVI_CHUNK);

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
