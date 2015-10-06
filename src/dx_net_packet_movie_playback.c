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

#include "dx_net_packet_file.h"

#include <stdio.h>    // For FILE, fopen, fclose
#include <stddef.h>   // For NULL
#include <string.h>   // For memcpy
#include <sys/socket.h>
#include <stdlib.h>   // For malloc
#include <fcntl.h>    // For read, write
#include <stdint.h>   // For uint32_t, ...
#include <sys/stat.h> // For stat
#include <dirent.h>   // For DIR, opendir, ...
#include <dx_net_packet_movie_playback.h>
#include <dx_net_packet_movie_playback.h>
#include <errno.h>    // For errno

#include "dx.h"

#include "dx_debug_assert.h"
#include "dx_debug_malloc.h"

#include "dx_util_log.h"

#include "dx_file_movie.h"
#include "dx_file_avi.h"

#include "dx_net_packet.h"
#include "dx_net_packet_io.h"
#include "dx_net_packet_movie_playback.h"

int dx_packet_send_movie_get_info(int fd, char* path) {
	dx_packet_movie_get_info_t* packet;
	uint32_t len = sizeof(dx_data_movie_get_info_t);

	packet = (dx_packet_movie_get_info_t*)MALLOC(len);

	dx_packet_set_header((dx_packet_t*)packet, len, DX_PACKET_TYPE_MOVIE, DX_MOVIE_GET_INFO, DX_DATA_TYPE_MOVIE_GET_INFO);

	strncpy((char*)&(packet->data.path), path, DX_PATH_MAX_SIZE);

	dx_write(fd, packet, len, 0);

	return 0;
}

int dx_packet_send_movie_info(int fd, char* path, dx_movie_context_t* context) {
	dx_packet_movie_info_t* packet;
	uint32_t len = 0;
	int i = 0;

	len = DX_PACKET_MOVIE_INFO_SIZE(context->track_count);

	packet = (dx_packet_movie_info_t*)MALLOC(len);

	dx_packet_set_header((dx_packet_t*)packet, len, DX_PACKET_TYPE_MOVIE, DX_MOVIE_INFO, DX_DATA_TYPE_MOVIE_INFO);

	strncpy((char*)&(packet->data.path), path, DX_PATH_MAX_SIZE);
	packet->data.total_frame = htonl(context->total_frame);
	packet->data.playtime = htons(context->playtime);
	packet->data.total_fragment = htonl(context->total_fragment);
	packet->data.frames_per_sec = htons(context->framerate != 0 ? (1000000 / context->framerate) : 0);
	packet->data.track_count = context->track_count;
	packet->data.width = htons(context->width);
	packet->data.height = htons(context->height);

	for(i = 0;i < context->track_count;i++) {
		dx_data_movie_track_info_t* packet_track = packet->data.track_info + i;
		dx_movie_track_info_t* context_track = context->track_info + i;

		memcpy(packet_track->id, context_track->id, 4);
		memcpy(packet_track->type, context_track->type, 4);
	}

	dx_write(fd, packet, len, 0);

	return 0;
}

int dx_packet_send_movie_frame(int fd, dx_movie_context_t* context) {
	dx_packet_movie_frame_t* packet;
	uint32_t len = 0;
	int i = 0;

	dx_movie_frame_index_t* current_frame;
	uint8_t track_count = 0;
	uint32_t offset = 0;
	uint32_t frame_offset_base;

	/*
	 * 현재 프레임을 만들어낸다.
	 */

	current_frame = dx_avi_get_frame_index(context); /* 현재 프레임의 트랙 인덱스 정보를 채운다. */
	track_count = current_frame->track_count;

	/* 패킷의 크기를 구한다. */
	frame_offset_base = DX_PACKET_MOVIE_FRAME_SIZE(track_count, 0);
	len = DX_PACKET_MOVIE_FRAME_SIZE(track_count, current_frame->frame_length);

	packet = (dx_packet_movie_frame_t*)MALLOC(len);

	dx_packet_set_header((dx_packet_t*)packet, len, DX_PACKET_TYPE_MOVIE, DX_MOVIE_FRAME, DX_DATA_TYPE_MOVIE_FRAME);

	packet->data.frameno = htonl(current_frame->frame_no);
	packet->data.index_count = track_count;
	packet->data.flags = 0;
	packet->data.frame_length = htonl(current_frame->frame_length);
	strncpy((char*)&(packet->data.path), context->path, DX_PATH_MAX_SIZE);

	for(i = 0;i < track_count;i++) {
		dx_data_movie_track_index_t* packet_index = packet->data.track_index + i;
		dx_movie_frame_track_index_t* movie_index = current_frame->track + i;

		packet_index->flags = (char)(movie_index->flag); /* TODO i-frame / p-frame 같은 정보를 담아야 한다. */
		packet_index->length = htonl(movie_index->length);
		packet_index->offset = htonl(offset);
		memcpy(packet_index->track_id, movie_index->track_id, 4);

		offset += movie_index->length;
	}
	{
		int frame_length = dx_avi_get_frame_data(context, ((int8_t*)packet) + frame_offset_base);
		if(current_frame->frame_length != frame_length) {
			ERROR("Frame Data Length is not correct.(%d vs %d)", frame_length, current_frame->frame_length);
		}
	}

	dx_write(fd, packet, len, 1 /* discardable */);

	return 0;
}

int dx_packet_send_movie_start(int fd, char* path, uint32_t start_frame, uint32_t stop_frame, uint16_t frames_per_sec) {
	dx_packet_movie_command_t* packet = (dx_packet_movie_command_t*)MALLOC(DX_PACKET_MOVIE_COMMAND_I_SIZE);

	dx_packet_set_header((dx_packet_t*)packet, DX_PACKET_MOVIE_COMMAND_I_SIZE, DX_PACKET_TYPE_MOVIE, DX_MOVIE_COMMAND_START, DX_DATA_TYPE_MOVIE_COMMAND_I);

	strncpy((char*)packet->data.path, path, DX_PATH_MAX_SIZE);
	packet->data.start_frame = htonl(start_frame);
	packet->data.stop_frame = htonl(stop_frame);
	packet->data.frames_per_sec = htons(frames_per_sec);

	dx_write(fd, packet, DX_PACKET_MOVIE_COMMAND_I_SIZE, 0);

	return 0;
}

int dx_packet_send_movie_stop(int fd, char* path) {
	dx_packet_movie_command_t* packet = (dx_packet_movie_command_t*)MALLOC(DX_PACKET_MOVIE_COMMAND_II_SIZE);

	dx_packet_set_header((dx_packet_t*)packet, DX_PACKET_MOVIE_COMMAND_II_SIZE, DX_PACKET_TYPE_MOVIE, DX_MOVIE_COMMAND_STOP, DX_DATA_TYPE_MOVIE_COMMAND_II);

	strncpy((char*)packet->data.path, path, DX_PATH_MAX_SIZE);

	dx_write(fd, packet, DX_PACKET_MOVIE_COMMAND_II_SIZE, 0);

	return 0;
}

int dx_packet_send_movie_resume(int fd, char* path, uint32_t start_frame, uint32_t stop_frame, uint16_t frames_per_sec) {
	dx_packet_movie_command_t* packet = (dx_packet_movie_command_t*)MALLOC(DX_PACKET_MOVIE_COMMAND_I_SIZE);

	dx_packet_set_header((dx_packet_t*)packet, DX_PACKET_MOVIE_COMMAND_I_SIZE, DX_PACKET_TYPE_MOVIE, DX_MOVIE_COMMAND_RESUME, DX_DATA_TYPE_MOVIE_COMMAND_I);

	strncpy((char*)packet->data.path, path, DX_PATH_MAX_SIZE);
	packet->data.start_frame = htonl(start_frame);
	packet->data.stop_frame = htonl(stop_frame);
	packet->data.frames_per_sec = htons(frames_per_sec);

	dx_write(fd, packet, DX_PACKET_MOVIE_COMMAND_I_SIZE, 0);

	return 0;
}

int dx_packet_send_movie_pause(int fd, char* path) {
	dx_packet_movie_command_t* packet = (dx_packet_movie_command_t*)MALLOC(DX_PACKET_MOVIE_COMMAND_II_SIZE);

	dx_packet_set_header((dx_packet_t*)packet, DX_PACKET_MOVIE_COMMAND_II_SIZE, DX_PACKET_TYPE_MOVIE, DX_MOVIE_COMMAND_PAUSE, DX_DATA_TYPE_MOVIE_COMMAND_II);

	strncpy((char*)packet->data.path, path, DX_PATH_MAX_SIZE);

	dx_write(fd, packet, DX_PACKET_MOVIE_COMMAND_II_SIZE, 0);

	return 0;
}

