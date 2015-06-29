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

#ifndef __DX_FILE_AVI_H
#define __DX_FILE_AVI_H

#include "dx_file_movie.h"

typedef struct {
	char		cc[4];
	uint32_t	size;
	int8_t		data[0];
} dx_avi_chunk_t;

typedef struct {
	char		type[4];
	uint32_t	size;
	char		cc[4];
	int8_t		data[0];
} dx_avi_list_t;

typedef struct {
	char		ckid[4]; /* chunk id */
	uint32_t	flags;
	uint32_t	offset;
	uint32_t	length;
} dx_avi_index_entry_t;

#define DX_AVI_CHUNK_SIZE(sz) (sizeof(dx_avi_chunk_t) + sz)
#define DX_AVI_LIST_SIZE(sz) (sizeof(dx_avi_list_t) + sz - 4)

typedef struct {
	uint32_t	framerate;
	uint32_t	max_transfer_rate;
	uint32_t	padding_granularity;

	uint32_t	flags;
	uint32_t	total_frames; 		/* this value should not be considered reliable. */
	uint32_t	initial_frames; 	/* ignore */
	uint32_t	streams;			/* track count */
	uint32_t	suggested_buffer_size;

	uint32_t	width;
	uint32_t	height;

	uint32_t	reserved[4];
} dx_avi_main_header_t;

typedef struct {
	char	type[4];
	char	handler[4];
	uint32_t	flags;
	uint16_t	priority;
	uint16_t	language;
	uint32_t	initial_frames;
	uint32_t	scale;
	uint32_t	rate;
	uint32_t	start;
	uint32_t	length;
	uint32_t	suggested_buffer_size;
	uint32_t	quality;
	uint32_t	sample_size;
	uint32_t	frame_x;
	uint32_t	frame_y;
	uint32_t	frame_w;
	uint32_t	frame_h;
} dx_avi_track_header_t;

/*
 * APIs
 */

dx_movie_context_t* dx_avi_parse_scheme(int fd);

int dx_avi_info(char* path);
int dx_avi_open(char* path);
int dx_avi_is_valid_chunk(dx_avi_chunk_t* chunk);
//int dx_avi_find_index_chunk(int fd, dx_avi_chunk_t* chunk, void* clojure);

//void dx_avi_chunk_print(dx_avi_chunk_t* chunk);
//void dx_avi_list_print(dx_avi_list_t* list);
//void dx_avi_index_print(dx_avi_index_entry_t* index);

//int dx_avi_find_index_by_frame_no(dx_movie_context_t* context, int nframe);

int dx_avi_seek_frame(dx_movie_context_t* context, int offset, int whence);
dx_movie_frame_index_t* dx_avi_get_frame_index(dx_movie_context_t* context);
int dx_avi_get_frame_data(dx_movie_context_t* context, int8_t* buffer);

#endif /* __DX_FILE_AVI_H */
