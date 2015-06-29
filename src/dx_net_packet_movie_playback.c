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

#include "dx_file_movie.h"

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

	FREE(packet);

	return 0;
}

int dx_packet_send_movie_info(int fd, char* path, dx_movie_context_t* context) {
	dx_packet_movie_info_t* packet;
	uint32_t len = 0;
	int i = 0;

	len = DX_PACKET_MOVIE_INFO_SIZE(context->track_count);

	packet = (dx_packet_movie_info_t*)MALLOC(len);

	dx_packet_set_header((dx_packet_t*)packet, len, DX_PACKET_TYPE_MOVIE, DX_MOVIE_GET_INFO, DX_DATA_TYPE_MOVIE_GET_INFO);

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

	FREE(packet);

	return 0;
}

int dx_packet_send_movie_frame(int fd, dx_movie_context_t* context, int offset, int whence) {
	dx_packet_movie_frame_t* packet;
	uint32_t len = 0;
	int data_size = 0;
	int i = 0;


//
//
//	len = DX_PACKET_MOVIE_FRAME_SIZE(context->track_count, data_size);
//
//	packet = (dx_packet_movie_frame_t*)MALLOC(len);
//
//	dx_packet_set_header((dx_packet_t*)packet, len, DX_PACKET_TYPE_MOVIE, DX_MOVIE_GET_INFO, DX_DATA_TYPE_MOVIE_GET_INFO);
//
//	strncpy((char*)&(packet->data.path), path, DX_PATH_MAX_SIZE);
//	packet->data.total_frame = htonl(context->total_frame);
//	packet->data.playtime = htons(context->playtime);
//	packet->data.total_fragment = htonl(context->total_fragment);
//	packet->data.frames_per_sec = htons(context->framerate != 0 ? (1000000 / context->framerate) : 0);
//	packet->data.track_count = context->track_count;
//	packet->data.width = htons(context->width);
//	packet->data.height = htons(context->height);
//
//	for(i = 0;i < context->track_count;i++) {
//		dx_data_movie_track_info_t* packet_track = packet->data.track_info + i;
//		dx_movie_track_info_t* context_track = context->track_info + i;
//
//		memcpy(packet_track->id, context_track->id, 4);
//		memcpy(packet_track->type, context_track->type, 4);
//	}
//
//	dx_write(fd, packet, len, 0);
//
//	FREE(packet);

	return 0;
}

int dx_packet_send_movie_start(int fd, char* path) {
	return 0;
}

int dx_packet_send_movie_stop(int fd, char* path) {
	return 0;
}

int dx_packet_send_movie_resume(int fd, char* path) {
	return 0;
}

int dx_packet_send_movie_pause(int fd, char* path) {
	return 0;
}

