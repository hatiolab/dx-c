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

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>		// For lseek, read, close
#include <stdint.h>		// For uint32_t, ...
#include <string.h>
#include <arpa/inet.h>

#include "dx_debug_assert.h"
#include "dx_debug_malloc.h"

#include "dx_util_log.h"

#include "dx_file_movie.h"
#include "dx_file_avi.h"

typedef struct dx_avi_chunk_map dx_avi_chunk_map_t;
typedef int (*dx_avi_scheme_handler)(int fd, dx_avi_chunk_t* chunk, dx_avi_chunk_map_t* map, void* clojure);

struct dx_avi_chunk_map {
	char	type[4];
	dx_avi_scheme_handler handler;
};

int dx_avi_traverse_chunk_tree(int fd, dx_avi_chunk_map_t* map, void* clojure) {
	dx_avi_chunk_t chunk;
	int i = 0;

	read(fd, &chunk, sizeof(dx_avi_chunk_t));

	while(map[i].handler != NULL) {
		if(strncmp(chunk.cc, map[i].type, 4) == 0) {
			return map[i].handler(fd, &chunk, map, clojure);
		}
		i++;
	}
	return -1;
}

/*
 * AVI movie internal list/chunk structure traverse handlers..
 */
int dx_avi_riff_handler(int fd, dx_avi_chunk_t* chunk, dx_avi_chunk_map_t* map, void* clojure);
int dx_avi_chunk_avih_handler(int fd, dx_avi_chunk_t* chunk, dx_avi_chunk_map_t* map, void* clojure);
int dx_avi_chunk_strh_handler(int fd, dx_avi_chunk_t* chunk, dx_avi_chunk_map_t* map, void* clojure);
int dx_avi_chunk_strn_handler(int fd, dx_avi_chunk_t* chunk, dx_avi_chunk_map_t* map, void* clojure);
int dx_avi_chunk_idx1_handler(int fd, dx_avi_chunk_t* chunk, dx_avi_chunk_map_t* map, void* clojure);

int dx_avi_list_handler(int fd, dx_avi_chunk_t* chunk, dx_avi_chunk_map_t* map, void* clojure);
int dx_avi_chunk_handler(int fd, dx_avi_chunk_t* chunk, dx_avi_chunk_map_t* map, void* clojure);

dx_avi_chunk_map_t dx_avi_scheme_parser_map[] = {
	{ "RIFF", dx_avi_list_handler },
	{ "LIST", dx_avi_list_handler },
	{ "JUNK", dx_avi_chunk_handler },
	{ "avih", dx_avi_chunk_avih_handler },
	{ "indx", dx_avi_chunk_handler },
	{ "strh", dx_avi_chunk_strh_handler },
	{ "strf", dx_avi_chunk_handler },
//	{ "strn", dx_avi_chunk_strn_handler }, /* 샘플파일에서 이 청크를 찾을 수 없었음. */
	{ "strn", dx_avi_chunk_handler },
	{ "idx1", dx_avi_chunk_idx1_handler },
//	{ "00dc", dx_avi_chunk_handler },
//	{ "01dc", dx_avi_chunk_handler },
//	{ "02wb", dx_avi_chunk_handler },
//	{ "03st", dx_avi_chunk_handler },
//	{ "04st", dx_avi_chunk_handler },
	{ "    ", NULL },
};

dx_movie_context_t* dx_avi_parse_scheme(int fd) {
	dx_movie_context_t* context;

	/* 동영상 파일의 맨 처음으로 이동 */
	lseek(fd, 0, SEEK_SET);

	/* AVI파일의 CHUNK트리를 traverse하면서 context에 값을 채운다 */
	if(-1 == dx_avi_traverse_chunk_tree(fd, dx_avi_scheme_parser_map, &context)) {
		ERROR("Parsing AVI movie file failed.");
		return NULL;
	}

	return context;
}

void dx_avi_chunk_print(dx_avi_chunk_t* chunk) {
	CONSOLE("CHUNK %-6.4s (%d bytes)\n", chunk->cc, chunk->size);
}

void dx_avi_list_print(dx_avi_list_t* list) {
	CONSOLE("%-6.4s %-6.4s(%d bytes)\n", list->type, list->cc, list->size);
}

void dx_avi_index_print(dx_avi_index_entry_t* index) {
	CONSOLE("INDEX %-6.4s (%d bytes from %d)\n", index->ckid, index->length, index->offset);
}

int dx_avi_info(char* path) {
	int fd = open(path, O_RDONLY);
	dx_avi_list_t list;

	read(fd, &list, sizeof(dx_avi_list_t));

	return 0;
}

int file_avi_read_chunk(int fd, dx_avi_chunk_map_t* map, void* clojure) {
	dx_avi_chunk_t chunk;
	int i = 0;

	read(fd, &chunk, sizeof(dx_avi_chunk_t));

	while(map[i].handler != NULL) {
		if(strncmp(chunk.cc, map[i].type, 4) == 0) {
			return map[i].handler(fd, &chunk, map, clojure);
		}
		i++;
	}
	return -1;
}

/*
 * 프레임번호로 프레임을 찾는 방법은 두가지가 있다.
 * 1. 인덱스(idx1)청크에서 찾는 방법.
 * 2. 무비 리스트(movi)에서 직접 찾는 방법.
 *
 * 여기에서는 인덱스 청크에서 찾는 방법을 제공한다.
 * 찾는 방법 -
 * 찾고자 하는 프레임 번호를 n이라고 하면,
 * 0번째 인덱스 청크부터 순차적으로 뒤져가면서, 트랙이름이 "00"으로 시작하는 n번째의 청크의 인덱스를 반환한다.
 */

//int dx_avi_find_index_by_frame_no(dx_movie_context_t* context, int nframe) {
//	int i = 0;
//	int found_frame = 0;
//	char track_name[4];
//
//	for(i = 0;found_frame < context->total_frame;i++) {
//		lseek(context->fd, context->index_offset + (i * sizeof(dx_avi_index_entry_t)), SEEK_SET);
//		read(context->fd, track_name, 4);
//		if(memcmp(track_name, "00", 2) == 0 && nframe == ++found_frame)
//			return i;
//	}
//
//	return -1;
//}

/*
 * 현재 인덱스를 옮김.
 */
int dx_avi_seek_frame(dx_movie_context_t* context, int offset, int whence) {
	int frame_no;
	int i = 0;
	int found_frame = 0;
	char track_name[4];

	ASSERT("Movie Context 초기화가 되어있지 않습니다.", context->current_frame != NULL)

	switch(whence) {
	case SEEK_SET:
		/*
		 * int dx_avi_find_index_by_frame_no(dx_movie_context_t* context, int nframe);
		 * 를 사용해서 인덱스 NO를 찾는다.
		 * 위 함수도 whence를 사용하도록 수정한다.
		 */
		frame_no = offset;
		break;
	case SEEK_CUR:
		frame_no = context->current_frame->frame_no + offset;
		break;
	case SEEK_END:
		frame_no = context->total_frame + offset;
		break;
	}

	for(i = 0;found_frame < context->total_frame;i++) {
		lseek(context->fd, context->index_offset + (i * sizeof(dx_avi_index_entry_t)), SEEK_SET);
		read(context->fd, track_name, 4);
		if(memcmp(track_name, "00", 2) == 0 && frame_no == ++found_frame) {
			context->current_frame->frame_no = frame_no;
			context->current_frame->fragment_no = i;
			return i;
		}
	}

	return -1;
}

/*
 * 현재 인덱스에서 프레임을 가져오고, 인덱스를 하나 증가시킴.
 */
dx_movie_frame_index_t* dx_avi_get_frame(dx_movie_context_t* context) {

	int i = 0;
	int current_fragment_no = context->current_frame->fragment_no;
	int current_frame_no = context->current_frame->frame_no;
	dx_avi_index_entry_t entry;

	/*
	 * 현재 프레임의 fragment_no부터 다음 프레임까지 읽어서, 현재 프레임의 인덱스 리스트에 값을 채운다.
	 * 현재 프레임의 frame_no와 fragment_no를 증가시킨다.
	 */

	memset(context->current_frame->track, 0x0, sizeof(dx_movie_frame_track_index_t)*context->track_count);

	for(i = 0;i < context->track_count && current_fragment_no + i < context->total_fragment;i++) {

		dx_movie_frame_track_index_t* index = context->current_frame->track + i;
		read(context->fd, &entry, sizeof(entry));

		if(i != 0 && memcmp(entry.ckid, "00", 2) == 0)
			break;

		memcpy(index->track_id, entry.ckid, 4);
		index->length = entry.length;
		index->offset = entry.offset;
	}

	if(context->current_frame->frame_no < context->total_frame)
		context->current_frame->frame_no++;

	if(context->current_frame->fragment_no < context->total_fragment)
		context->current_frame->fragment_no += i;
	else
		context->current_frame->fragment_no = context->total_fragment;

	return context->current_frame;
}

/*
 * AVI File List/Chunk Traverse handlers
 *
 */

int dx_avi_riff_handler(int fd, dx_avi_chunk_t* chunk, dx_avi_chunk_map_t* map, void* clojure) {
	dx_avi_list_t list;

	memcpy(list.type, chunk->cc, 4);
	list.size = chunk->size;

	read(fd, list.cc, 4);

	dx_avi_list_print(&list);
	return DX_AVI_LIST_SIZE(chunk->size);
}

int dx_avi_chunk_avih_handler(int fd, dx_avi_chunk_t* chunk, dx_avi_chunk_map_t* map, void* clojure) {
	dx_avi_main_header_t header;
	dx_movie_context_t* context = clojure;
	off_t offset;

	offset = lseek(fd, 0, SEEK_CUR);

	read(fd, &header, sizeof(header));

	context = (dx_movie_context_t*)MALLOC(DX_MOVIE_CONTEXT_SIZE(header.streams));
	memset(context, 0x0, DX_MOVIE_CONTEXT_SIZE(header.streams));

	context->fd = fd;
	context->framerate = header.framerate;
	context->track_count = header.streams;
	context->width = header.width;
	context->height = header.height;

	*((dx_movie_context_t**)clojure) = context;
	context->header_offset = offset; /* chunk->data의 오프셋임 */

	dx_movie_frame_index_t* current_frame;

	context->current_frame = (dx_movie_frame_index_t*)MALLOC(DX_MOVIE_FRAME_INDEX_SIZE(context->track_count));
	memset(context->current_frame, 0x0, DX_MOVIE_FRAME_INDEX_SIZE(context->track_count));

	return DX_AVI_CHUNK_SIZE(chunk->size);
}

int dx_avi_chunk_strh_handler(int fd, dx_avi_chunk_t* chunk, dx_avi_chunk_map_t* map, void* clojure) {
	dx_avi_track_header_t header;

	dx_movie_context_t** pcontext = (dx_movie_context_t**)clojure;
	dx_movie_track_info_t* track_info;
	dx_movie_frame_track_index_t* track_index;

	int i = 0;

	read(fd, &header, sizeof(header));

	for(i = 0;i < (*pcontext)->track_count;i++) {
		track_info = (*pcontext)->track_info;
		track_index = (*pcontext)->current_frame->track + i;

		if(track_info[i].id[0] != 0)
			continue;

		/* Set Track Information.. */

//		if(strncmp(header.type, "vids", 4) == 0)
//			sprintf(track_info[i].id, "%02ddc", i);
//		else if(strncmp(header.type, "auds", 4) == 0)
//			sprintf(track_info[i].id, "%02dwb", i);
//		else if(strncmp(header.type, "txts", 4) == 0)
//			sprintf(track_info[i].id, "%02dst", i);

		sprintf(track_info[i].id, "%02d", i);
		memcpy(track_info[i].type, header.type, 4);
		memcpy(track_info[i].handler, header.handler, 4);
		track_info[i].framerate = (header.scale != 0) ? header.rate / header.scale : header.rate;


		break;
	}

	CONSOLE("\nType : %.4s\n", header.type);
	CONSOLE("Handler : %.4s\n", header.handler);
	CONSOLE("Flags : %#x\n", header.flags);
	CONSOLE("Language : %d\n", header.language);
	CONSOLE("Initial Frames : %d\n", header.initial_frames);
	CONSOLE("Scale : %d\n", header.scale);
	CONSOLE("Rate : %d\n", header.rate);
	CONSOLE("Start : %d\n", header.start);
	CONSOLE("Length : %d\n", header.length);
	CONSOLE("Suggested Buffer Size : %d\n", header.suggested_buffer_size);
	CONSOLE("Quality : %d\n", header.quality);
	CONSOLE("Sample Size : %d\n", header.sample_size);
	CONSOLE("Frame LEFT : %d\n", header.frame_x);
	CONSOLE("Frame TOP : %d\n", header.frame_y);
	CONSOLE("Frame RIGHT : %d\n", header.frame_w);
	CONSOLE("Frame BOTTOM : %d\n\n", header.frame_h);

	return DX_AVI_CHUNK_SIZE(chunk->size);
}

int dx_avi_chunk_strn_handler(int fd, dx_avi_chunk_t* chunk, dx_avi_chunk_map_t* map, void* clojure) {

	char track_name[4];

	dx_movie_context_t** pcontext = (dx_movie_context_t**)clojure;
	dx_movie_track_info_t* track_info;
	int i = 0;

	read(fd, &track_name, sizeof(track_name));

	for(i = 0;i < (*pcontext)->track_count;i++) {
		track_info = (*pcontext)->track_info;
		if(track_info[i].id[2] != 0)
			continue;

		/* Set Track Information.. */

		memcpy(track_info[i].id, track_name, 4);

		break;
	}

	CONSOLE("\nType : %.4s\n", track_name);

	return DX_AVI_CHUNK_SIZE(chunk->size);
}

int dx_avi_chunk_idx1_handler(int fd, dx_avi_chunk_t* chunk, dx_avi_chunk_map_t* map, void* clojure) {
	dx_avi_index_entry_t entry;
	dx_movie_context_t** pcontext = (dx_movie_context_t**)clojure;
	int i = 0;
	int frame_count = 0;
	int total_fragment = 0;
	char base_chunk_name[4];

	(*pcontext)->index_offset = lseek(fd, 0, SEEK_CUR); /* chunk(idx1)->data의 오프셋임 */

	memcpy(base_chunk_name, (*pcontext)->track_info[0].id, 4);

	total_fragment = chunk->size / sizeof(dx_avi_index_entry_t);

	for(i = 0;i < total_fragment;i++) {
		read(fd, &entry, sizeof(dx_avi_index_entry_t));
		LOG("Index [%d] %.4s %d %d %d", i, entry.ckid, entry.flags, entry.offset, entry.length);

		if(strncmp(base_chunk_name, entry.ckid, 2) == 0)
			frame_count++;
	}

	(*pcontext)->total_fragment = total_fragment;
	(*pcontext)->total_frame = frame_count;
	(*pcontext)->playtime = frame_count / (1000000 / (*pcontext)->framerate);

	dx_avi_chunk_print(chunk);

	return DX_AVI_CHUNK_SIZE(chunk->size);
}

int dx_avi_list_handler(int fd, dx_avi_chunk_t* chunk, dx_avi_chunk_map_t* map, void* clojure) {
	dx_avi_list_t list;
	int nread;
	off_t pos;
	off_t eof_pos;

	memcpy(list.type, chunk->cc, 4);
	list.size = chunk->size;

	read(fd, list.cc, 4);

	dx_avi_list_print(&list);

	pos = lseek(fd, 0, SEEK_CUR);
	eof_pos = pos + list.size;

	if(memcmp(list.cc, "movi", 4) == 0) {
		dx_movie_context_t** pcontext = (dx_movie_context_t**)clojure;
		(*pcontext)->frame_offset = pos; /* list(movi)->data의 오프셋임 */
	}

	/* traverse child chunks */
	while(pos < eof_pos) {
		nread = file_avi_read_chunk(fd, map, clojure);
		if(nread < 0)
			break;
		pos += nread;
		lseek(fd, pos, SEEK_SET);
	}

	return DX_AVI_LIST_SIZE(chunk->size);
}

int dx_avi_chunk_handler(int fd, dx_avi_chunk_t* chunk, dx_avi_chunk_map_t* map, void* clojure) {
	dx_avi_chunk_print(chunk);

	return DX_AVI_CHUNK_SIZE(chunk->size);
}

//int dx_avi_find_index_chunk(int fd, dx_avi_chunk_t* chunk, void* clojure) {
//	off_t offset;
//	int nread;
//	dx_avi_list_t top;
//	dx_avi_chunk_t tmp;
//
//	lseek(fd, 0, SEEK_SET);
//	read(fd, &top, sizeof(dx_avi_list_t));
//	offset = lseek(fd, sizeof(dx_avi_list_t), SEEK_SET);
//
//	while(offset < DX_AVI_LIST_SIZE(top.size)) {
//		nread = read(fd, &tmp, sizeof(dx_avi_chunk_t));
//
//		if(strncmp(tmp.cc, "idx1", 4) == 0) {
//			*chunk = tmp;
//			return offset;
//		}
//		if(dx_avi_is_valid_chunk(&tmp) < -1)
//			return -1;
//
//		offset += DX_AVI_CHUNK_SIZE(tmp.size);
//		offset = lseek(fd, offset, SEEK_SET);
//	}
//
//	return -1;
//}
//
//int dx_avi_find_movie_list(int fd, dx_avi_list_t* list, void* clojure) {
//	off_t offset;
//	int nread;
//	dx_avi_list_t top;
//	dx_avi_list_t tmp;
//
//	lseek(fd, 0, SEEK_SET);
//	read(fd, &top, sizeof(dx_avi_list_t));
//	offset = lseek(fd, sizeof(dx_avi_list_t), SEEK_SET);
//
//	while(offset < DX_AVI_LIST_SIZE(top.size)) {
//		nread = read(fd, &tmp, sizeof(dx_avi_list_t));
//
//		if(strncmp(tmp.cc, "movi", 4) == 0) {
//			*list = tmp;
//			return offset;
//		}
//		if(dx_avi_is_valid_chunk(&tmp) < -1)
//			return -1;
//
//		offset += DX_AVI_CHUNK_SIZE(tmp.size);
//		offset = lseek(fd, offset, SEEK_SET);
//	}
//
//	return -1;
//}

