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

#ifndef __DX_NET_PACKET_MOVIE_H
#define __DX_NET_PACKET_MOVIE_H

#include <stdint.h> // For uint_16_t, ...
#include <time.h> 	// For time_t

#include "dx_file_movie.h"

#include "dx_net_packet.h"

/*
 * Definitions
 */

#define DX_TRACK_ID_SIZE	4
#define DX_TRACK_TYPE_SIZE	4

typedef struct {
	int8_t   	path[DX_PATH_MAX_SIZE];
} __attribute__((__packed__)) dx_data_movie_get_info_t;

typedef struct {
	dx_packet_header_t header;
	dx_data_movie_get_info_t data;
} __attribute__((__packed__)) dx_packet_movie_get_info_t;

typedef struct dx_data_movie_track_info {
	int8_t		id[DX_TRACK_ID_SIZE];
	int8_t		type[DX_TRACK_TYPE_SIZE];
} __attribute__((__packed__)) dx_data_movie_track_info_t;

typedef struct dx_data_movie_info {
	int8_t   	path[DX_PATH_MAX_SIZE];
	uint32_t	total_frame;
	uint32_t	total_fragment;
	uint16_t	frames_per_sec;
	uint16_t	playtime; /* seconds */
	uint16_t	width;
	uint16_t	height;

	uint8_t		track_count;
	dx_data_movie_track_info_t track_info[0];
} __attribute__((__packed__)) dx_data_movie_info_t;

typedef struct {
	dx_packet_header_t header;
	dx_data_movie_info_t data;
} __attribute__((__packed__)) dx_packet_movie_info_t;

#define DX_PACKET_MOVIE_INFO_SIZE(tracksz) (sizeof(dx_packet_movie_info_t)+((tracksz)*sizeof(dx_data_movie_track_info_t)))

typedef struct dx_data_movie_track_index {
	int8_t		track_id[DX_TRACK_ID_SIZE];
	uint32_t	offset;
	uint32_t	length;
	uint8_t		flags;
	uint8_t		reserved[3];
} __attribute__((__packed__)) dx_data_movie_track_index_t;

typedef struct dx_data_movie_frame {
	int8_t   	path[DX_PATH_MAX_SIZE];
	uint32_t	frameno;
	uint8_t		flags;
	uint8_t		index_count;

	dx_data_movie_track_index_t track_index[0];
	// track_index 이후에 동영상 프레임이 추가된다.
} __attribute__((__packed__)) dx_data_movie_frame_t;

typedef struct {
	dx_packet_header_t header;
	dx_data_movie_frame_t data;
} __attribute__((__packed__)) dx_packet_movie_frame_t;

#define DX_PACKET_MOVIE_FRAME_SIZE(tracksz,datasz) (sizeof(dx_packet_movie_frame_t)+((tracksz)*sizeof(dx_data_movie_track_index_t)+(datasz)))

/*
 * movie command : start, resume
 */
typedef struct dx_data_movie_command {
	int8_t   	path[DX_PATH_MAX_SIZE];
	uint16_t	start_frame;
	uint16_t	stop_frame;
	uint16_t	frames_per_sec;
	uint8_t		flags;
	uint8_t		reserved;
} __attribute__((__packed__)) dx_data_movie_command_t;

typedef struct {
	dx_packet_header_t header;
	dx_data_movie_command_t data;
} __attribute__((__packed__)) dx_packet_movie_command_t;

typedef dx_data_movie_get_info_t dx_data_movie_pause_t;
typedef dx_data_movie_get_info_t dx_data_movie_stop_t;

/* Functions */

int dx_packet_send_movie_get_info(int fd, char* path);
int dx_packet_send_movie_info(int fd, char* path, dx_movie_context_t* context);

int dx_packet_send_movie_frame(int fd, dx_movie_context_t* context, int offset, int whence);

int dx_packet_send_movie_start(int fd, char* path);
int dx_packet_send_movie_stop(int fd, char* path);
int dx_packet_send_movie_pause(int fd, char* path);
int dx_packet_send_movie_resume(int fd, char* path);

#endif /* __DX_NET_PACKET_MOVIE_H */
