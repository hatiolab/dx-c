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

typedef struct dx_movie_context dx_movie_context_t;
typedef struct dx_movie_track_info dx_movie_track_info_t;
typedef struct dx_movie_fragment_track dx_movie_fragment_track_t;
typedef struct dx_movie_fragment dx_movie_fragment_t;

typedef void (*dx_movie_find_fragment)(dx_movie_context_t* context, int frame_no, dx_movie_fragment_t* fragment);

typedef struct dx_movie_track_info {
	char id[4];
	int encode_type;
};

struct dx_movie_context {
	int fd;
	int total_frame;
	int framerate;
	int playtime; /* seconds */
	off_t header_offset;
	off_t frame_offset;
	off_t index_offset;
	int track_count;

	dx_movie_find_fragment tracker;

	dx_movie_track_info_t track_info[0];
};

struct dx_movie_fragment_track {
	char track_id[4];
	off_t offset;
	int length;
};

struct dx_movie_fragment {
	off_t offset;
	int length;
	dx_movie_fragment_track_t track[0];
};

void dx_movie_open(char* path, dx_movie_context_t* movie);

#endif /* __DX_FILE_MOVIE_H */
