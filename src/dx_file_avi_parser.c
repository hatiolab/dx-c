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

#include "dx.h"

#include "dx_debug_assert.h"
#include "dx_debug_malloc.h"

#include "dx_util_log.h"

#include "dx_file_movie.h"
#include "dx_file_avi.h"

typedef struct dx_avi_chunk_map dx_avi_chunk_map_t;
typedef struct dx_avi_list_map dx_avi_list_map_t;

typedef int (*dx_avi_scheme_chunk_handler)(int fd, dx_avi_chunk_t* chunk, void* clojure);
typedef int (*dx_avi_scheme_list_handler)(int fd, dx_avi_list_t* list, void* clojure);

struct dx_avi_chunk_map {
	char	type[4];
	dx_avi_scheme_chunk_handler handler;
};

struct dx_avi_list_map {
	char	type[4];
	dx_avi_scheme_list_handler handler;
};

/*
 * AVI movie internal list/chunk structure traverse handlers..
 */

int dx_avi_chunk_avih_handler(int fd, dx_avi_chunk_t* chunk, void* clojure);
int dx_avi_chunk_strh_handler(int fd, dx_avi_chunk_t* chunk, void* clojure);
int dx_avi_chunk_strn_handler(int fd, dx_avi_chunk_t* chunk, void* clojure);
int dx_avi_chunk_idx1_handler(int fd, dx_avi_chunk_t* chunk, void* clojure);

int dx_avi_list_movi_handler(int fd, dx_avi_list_t* list, void* clojure);
int dx_avi_list_odml_handler(int fd, dx_avi_list_t* list, void* clojure);
int dx_avi_list_info_handler(int fd, dx_avi_list_t* list, void* clojure);
int dx_avi_list_avi_handler(int fd, dx_avi_list_t* list, void* clojure);
int dx_avi_list_avix_handler(int fd, dx_avi_list_t* list, void* clojure);
int dx_avi_list_strl_handler(int fd, dx_avi_list_t* list, void* clojure);
int dx_avi_list_hdrl_handler(int fd, dx_avi_list_t* list, void* clojure);
int dx_avi_list_rec_handler(int fd, dx_avi_list_t* list, void* clojure);

int dx_avi_chunk_handler(int fd, dx_avi_chunk_t* chunk, void* clojure);

dx_avi_chunk_map_t dx_avi_chunk_parser_map[] = {
	{ "JUNK", dx_avi_chunk_handler },
	{ "avih", dx_avi_chunk_avih_handler },
	{ "indx", dx_avi_chunk_handler },
	{ "strh", dx_avi_chunk_strh_handler },
	{ "strf", dx_avi_chunk_handler },
	{ "strn", dx_avi_chunk_strn_handler }, /* 샘플파일에서 이 청크를 찾을 수 없었음. */
	{ "strn", dx_avi_chunk_handler },
	{ "idx1", dx_avi_chunk_idx1_handler },	// idx1 index Chunk: Contains a list of the offsets and chunk types of every chunk inside the LIST 'movi' chunk
	{ "    ", NULL },
};

dx_avi_list_map_t dx_avi_list_parser_map[] = {
	{ "movi", dx_avi_list_movi_handler },
	{ "odml", dx_avi_list_odml_handler },
	{ "INFO", dx_avi_list_info_handler },
	{ "AVI ", dx_avi_list_avi_handler },
	{ "AVIX", dx_avi_list_avix_handler },
	{ "strl", dx_avi_list_strl_handler },
	{ "hdrl", dx_avi_list_hdrl_handler },
	{ "rec ", dx_avi_list_rec_handler },
};

/*
 * AVI Chunk Tree를 traversing하면서, 동영상 파일 구조와 관련된 필요한 정보를 처리한다.
 */
int dx_avi_traverse_chunk_tree(int fd, dx_avi_chunk_map_t* chunk_handler_map, dx_avi_list_map_t* list_handler_map, void* clojure) {
	dx_avi_chunk_t chunk;
	int i = 0;

	read(fd, &chunk, sizeof(dx_avi_chunk_t));

	if(strncmp(chunk.cc, "RIFF", 4) == 0 || strncmp(chunk.cc, "LIST", 4) == 0) {
		dx_avi_list_t list;
		int i = 0;
		int nread = 0;
		off_t pos, pos_eol; /* current position, end of list position */

		memcpy(list.type, chunk.cc, 4);
		list.size = chunk.size;

		read(fd, list.cc, 4);

		pos = lseek(fd, 0, SEEK_CUR);
		pos_eol = pos + list.size;

		while(list_handler_map[i].handler != NULL) {
			if(strncmp(list.cc, list_handler_map[i].type, 4) == 0) {
				list_handler_map[i].handler(fd, &list, clojure);
			}
			i++;
		}

		lseek(fd, pos, SEEK_SET);

		/* traverse child chunks */
		while(pos < pos_eol) {
			nread = dx_avi_traverse_chunk_tree(fd, chunk_handler_map, list_handler_map, clojure);
			if(nread < 0)
				break;
			pos += nread;
			lseek(fd, pos, SEEK_SET);
		}

		return DX_AVI_LIST_SIZE(list.size);
	}

	while(chunk_handler_map[i].handler != NULL) {
		if(strncmp(chunk.cc, chunk_handler_map[i].type, 4) == 0) {
			chunk_handler_map[i].handler(fd, &chunk, clojure);
			break;
		}
		i++;
	}

	return DX_AVI_CHUNK_SIZE(chunk.size);
}

/*
 * AVI 파일의 구조를 분석한다.
 */
dx_movie_context_t* dx_avi_parse_scheme(int fd) {
	dx_movie_context_t* context;

	/* 동영상 파일의 맨 처음으로 이동 */
	lseek(fd, 0, SEEK_SET);

	/* AVI파일의 CHUNK트리를 traverse하면서 context에 값을 채운다 */
	if(-1 == dx_avi_traverse_chunk_tree(fd, dx_avi_chunk_parser_map, dx_avi_list_parser_map, &context)) {
		ERROR("Parsing AVI movie file failed.");
		return NULL;
	}

	/* 현재 프레임을 맨 처음 프레임으로 설정 */
	dx_avi_seek_frame(context, 0, SEEK_SET);

	return context;
}

void dx_avi_print_chunk(dx_avi_chunk_t* chunk) {
	CONSOLE("CHUNK %-6.4s (%d bytes)\n", chunk->cc, chunk->size);
}

void dx_avi_print_list(dx_avi_list_t* list) {
	CONSOLE("%-6.4s %-6.4s(%d bytes)\n", list->type, list->cc, list->size);
}

void dx_avi_print_index(dx_avi_index_entry_t* index) {
	CONSOLE("INDEX %-6.4s (%d bytes from %d)\n", index->ckid, index->length, index->offset);
}

void dx_avi_print_stream_header(dx_avi_track_header_t* header) {
	CONSOLE("[STREAM HEADER] Type(%.4s) Handler(%.4s) Rate(%d) Sample Size(%d)\n",
			header->type,
			header->handler,
			header->rate,
			header->sample_size
	);

//	CONSOLE("Flags : %#x\n", header.flags);
//	CONSOLE("Language : %d\n", header.language);
//	CONSOLE("Initial Frames : %d\n", header.initial_frames);
//	CONSOLE("Scale : %d\n", header.scale);
//	CONSOLE("Rate : %d\n", header.rate);
//	CONSOLE("Start : %d\n", header.start);
//	CONSOLE("Length : %d\n", header.length);
//	CONSOLE("Suggested Buffer Size : %d\n", header.suggested_buffer_size);
//	CONSOLE("Quality : %d\n", header.quality);
//	CONSOLE("Sample Size : %d\n", header.sample_size);
//	CONSOLE("Frame LEFT : %d\n", header.frame_x);
//	CONSOLE("Frame TOP : %d\n", header.frame_y);
//	CONSOLE("Frame RIGHT : %d\n", header.frame_w);
//	CONSOLE("Frame BOTTOM : %d\n\n", header.frame_h);
}

/*
 * AVI File List/Chunk Traverse handlers
 *
 */

int dx_avi_chunk_avih_handler(int fd, dx_avi_chunk_t* chunk, void* clojure) {
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

	context->current_frame = (dx_movie_frame_index_t*)MALLOC(DX_MOVIE_FRAME_INDEX_SIZE(context->track_count));
	memset(context->current_frame, 0x0, DX_MOVIE_FRAME_INDEX_SIZE(context->track_count));

	return DX_AVI_CHUNK_SIZE(chunk->size);
}

int dx_avi_chunk_strh_handler(int fd, dx_avi_chunk_t* chunk, void* clojure) {
	dx_avi_track_header_t header;

	dx_movie_context_t** pcontext = (dx_movie_context_t**)clojure;
	dx_movie_track_info_t* track_info;

	int i = 0;

	read(fd, &header, sizeof(header));

	for(i = 0;i < (*pcontext)->track_count;i++) {
		track_info = (*pcontext)->track_info;

		if(track_info[i].id[0] != 0)
			continue;

		/* Set Track Information.. */

		/*
		 * 타입과 트랙 아이디 부여하는 룰이 없다.
		 */
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

	dx_avi_print_stream_header(&header);

	return DX_AVI_CHUNK_SIZE(chunk->size);
}

int dx_avi_chunk_strn_handler(int fd, dx_avi_chunk_t* chunk, void* clojure) {

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

int dx_avi_chunk_idx1_handler(int fd, dx_avi_chunk_t* chunk, void* clojure) {
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

	dx_avi_print_chunk(chunk);

	return DX_AVI_CHUNK_SIZE(chunk->size);
}

int dx_avi_list_movi_handler(int fd, dx_avi_list_t* list, void* clojure) {
	dx_movie_context_t** pcontext = (dx_movie_context_t**)clojure;

	off_t pos = lseek(fd, 0, SEEK_CUR);

	if((*pcontext)->frame_offset == 0)
		(*pcontext)->frame_offset = pos + 4; /* list(movi)->data의 오프셋임. +4 bytes 의미는 알 수없음. */

	dx_avi_print_list(list);

	return DX_AVI_LIST_SIZE(list->size);
}

int dx_avi_list_odml_handler(int fd, dx_avi_list_t* list, void* clojure) {
	return 0;
}

int dx_avi_list_info_handler(int fd, dx_avi_list_t* list, void* clojure) {
	return 0;
}

int dx_avi_list_avi_handler(int fd, dx_avi_list_t* list, void* clojure) {
	return 0;
}

int dx_avi_list_avix_handler(int fd, dx_avi_list_t* list, void* clojure) {
	return 0;
}

int dx_avi_list_strl_handler(int fd, dx_avi_list_t* list, void* clojure) {
	return 0;
}

int dx_avi_list_hdrl_handler(int fd, dx_avi_list_t* list, void* clojure) {
	return 0;
}

int dx_avi_list_rec_handler(int fd, dx_avi_list_t* list, void* clojure) {
	return 0;
}

/*
 * General purpose AVI file Chunk / List traverse handler
 */

int dx_avi_chunk_handler(int fd, dx_avi_chunk_t* chunk, void* clojure) {
	dx_avi_print_chunk(chunk);

	return 0;
}
