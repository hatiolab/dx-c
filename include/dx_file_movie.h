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

#ifndef __DX_FILE_MOVIE_H
#define __DX_FILE_MOVIE_H

#include "dx_net_packet_file.h"

typedef struct dx_movie_context dx_movie_context_t;
typedef struct dx_movie_track_info dx_movie_track_info_t;
typedef struct dx_movie_fragment_track dx_movie_fragment_track_t;
typedef struct dx_movie_fragment dx_movie_fragment_t;

typedef struct dx_movie_frame_track_index {
	char	track_id[4];
	off_t	offset;
	uint32_t flag;
	int		length;
} dx_movie_frame_track_index_t;

typedef struct dx_movie_frame_index {
	uint32_t	frame_no;					/* 다음에 읽을 프레임 넘버 - 현재 프레임 넘버로 바꿔야 한다. */
	uint32_t	fragment_no;				/* 다음에 읽을 프레그먼트 넘버 - 현재 트랙 번호로 바꿔야 한다. */
	uint8_t		track_count;
	uint32_t	frame_length;
	dx_movie_frame_track_index_t track[0];	/* 마지막에 읽은 프레임 인덱스 정보 */
} dx_movie_frame_index_t;

#define DX_MOVIE_FRAME_INDEX_SIZE(sz) (sizeof(dx_movie_frame_index_t)+((sz)*sizeof(dx_movie_frame_track_index_t)))

//typedef dx_movie_frame_index_t* (*dx_movie_find_frame)(dx_movie_context_t* context, int frame_no);

struct dx_movie_track_info {
	char	id[4];
	char	type[4];
	char	handler[4];
	int		framerate;
};

struct dx_movie_context {
	int		fd;
	char	path[DX_PATH_MAX_SIZE];
	int		total_frame;
	int		total_fragment;
	int		framerate;
	int		playtime; /* seconds */
	off_t	header_offset;
	off_t	frame_offset;
	off_t	index_offset;
	int		track_count;

	int		width;
	int		height;

	dx_movie_frame_index_t* current_frame;

//	dx_movie_find_frame tracker;

	dx_movie_track_info_t track_info[0];
};

#define DX_MOVIE_CONTEXT_SIZE(sz) (sizeof(dx_movie_context_t)+((sz)*sizeof(dx_movie_track_info_t)))
#define DX_MOVIE_FRAME_INDEX_SIZE(sz) (sizeof(dx_movie_frame_index_t)+((sz)*sizeof(dx_movie_frame_track_index_t)))

dx_movie_context_t* dx_movie_context_create(char* path);
void dx_movie_context_destroy(dx_movie_context_t* context);

int dx_movie_seek_frame(dx_movie_context_t* context, int offset, int whence);

//dx_movie_frame_index_t* dx_movie_get_index_for_frame_no(dx_movie_context_t* context, int offset, int whence);

int dx_movie_frame_eof(dx_movie_context_t* context);

#endif /* __DX_FILE_MOVIE_H */
