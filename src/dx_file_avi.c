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

#include "dx_file_avi.h"

typedef int (*dx_avi_scheme_handler)(int fd, AVI_CHUNK* chunk);
typedef struct dx_avi_chunk_map {
	char	type[4];
	dx_avi_scheme_handler handler;
} dx_avi_chunk_map_t;

int dx_avi_riff_handler(int fd, AVI_CHUNK* chunk);
int dx_avi_list_handler(int fd, AVI_CHUNK* chunk);
int dx_avi_chunk_handler(int fd, AVI_CHUNK* chunk);
int dx_avi_chunk_idx1_handler(int fd, AVI_CHUNK* chunk);

dx_avi_chunk_map_t dx_avi_chunk_handler_map[] = {
	{ "RIFF", dx_avi_riff_handler },
	{ "LIST", dx_avi_list_handler },
	{ "JUNK", dx_avi_chunk_handler },
	{ "avih", dx_avi_chunk_handler },
	{ "indx", dx_avi_chunk_handler },
	{ "strh", dx_avi_chunk_handler },
	{ "strf", dx_avi_chunk_handler },
	{ "strn", dx_avi_chunk_handler },
	{ "idx1", dx_avi_chunk_idx1_handler },
	{ "    ", NULL },
};

void dx_avi_chunk_print(AVI_CHUNK* chunk) {
	char cc[5];
	memset(cc, 0x0, 5);
	memcpy(cc, chunk->cc, 4);

	printf("CHUNK %4s (%d bytes)\n", cc, chunk->size);
}

void dx_avi_list_print(AVI_LIST* list) {
	char cc[5], type[5];
	memset(cc, 0x0, 5);
	memset(type, 0x0, 5);

	memcpy(cc, list->cc, 4);
	memcpy(type, list->type, 4);

	LOG("%4s %4s (%d bytes)", type, cc, list->size);
}

void dx_avi_index_print(dx_avi_index_entry_t* index) {
	char ckid[5];
	memset(ckid, 0x0, 5);
	memcpy(ckid, (char*)&index->ckid, 4);

	LOG("INDEX %4s (%d bytes from %d)", ckid, index->length, index->offset);
}

int dx_avi_info(char* path) {
	int fd = open(path, O_RDONLY);
	AVI_LIST list;

	read(fd, &list, sizeof(AVI_LIST));

	return 0;
}

int dx_avi_open(char* path) {
	int fd = open(path, O_RDONLY);
	AVI_LIST list;

	read(fd, &list, sizeof(AVI_LIST));

	ASSERT("AVI List Type should be RIFF\n", strncmp(list.type, "RIFF", 4) == 0)
	ASSERT("AVI File Type should be AVI \n", strncmp(list.cc, "AVI ", 4) == 0)
	printf("Size of RIFF : %d\n\n", list.size);

	return fd;
}

int dx_avi_is_valid_chunk(AVI_CHUNK* chunk) {
	int i = 0;

	while(dx_avi_chunk_handler_map[i].handler != NULL) {
		if(strncmp(chunk->cc, dx_avi_chunk_handler_map[i].type, 4) == 0)
			return i;
		i++;
	}
	return -1;
}

int file_avi_read_chunk(int fd) {
	AVI_CHUNK chunk;
	int i = 0;

	read(fd, &chunk, sizeof(AVI_CHUNK));

	while(dx_avi_chunk_handler_map[i].handler != NULL) {
		if(strncmp(chunk.cc, dx_avi_chunk_handler_map[i].type, 4) == 0) {
			return dx_avi_chunk_handler_map[i].handler(fd, &chunk);
		}
		i++;
	}
	return -1;
}

int dx_avi_find_index_chunk(int fd, AVI_CHUNK* chunk) {
	off_t offset;
	int nread;
	AVI_LIST top;
	AVI_CHUNK tmp;

	lseek(fd, 0, SEEK_SET);
	read(fd, &top, sizeof(AVI_LIST));
	offset = lseek(fd, sizeof(AVI_LIST), SEEK_SET);

	while(offset < DX_AVI_LIST_SIZE(top.size)) {
		nread = read(fd, &tmp, sizeof(AVI_CHUNK));

		if(strncmp(tmp.cc, "idx1", 4) == 0) {
			*chunk = tmp;
			return offset;
		}
		if(dx_avi_is_valid_chunk(&tmp) < -1)
			return -1;

		offset += DX_AVI_CHUNK_SIZE(tmp.size);
		offset = lseek(fd, offset, SEEK_SET);
	}

	return -1;
}

int dx_avi_find_movie_list(int fd, AVI_LIST* list) {
	off_t offset;
	int nread;
	AVI_LIST top;
	AVI_LIST tmp;

	lseek(fd, 0, SEEK_SET);
	read(fd, &top, sizeof(AVI_LIST));
	offset = lseek(fd, sizeof(AVI_LIST), SEEK_SET);

	while(offset < DX_AVI_LIST_SIZE(top.size)) {
		nread = read(fd, &tmp, sizeof(AVI_LIST));

		if(strncmp(tmp.cc, "movi", 4) == 0) {
			*list = tmp;
			return offset;
		}
		if(dx_avi_is_valid_chunk(&tmp) < -1)
			return -1;

		offset += DX_AVI_CHUNK_SIZE(tmp.size);
		offset = lseek(fd, offset, SEEK_SET);
	}

	return -1;
}

int dx_avi_riff_handler(int fd, AVI_CHUNK* chunk) {
	AVI_LIST list;

	memcpy(list.type, chunk->cc, 4);
	list.size = chunk->size;

	read(fd, list.cc, 4);

	dx_avi_list_print(&list);
	return DX_AVI_LIST_SIZE(chunk->size);
}

int dx_avi_list_handler(int fd, AVI_CHUNK* chunk) {
	AVI_LIST list;
	int nread;
	off_t pos;
	off_t eof_pos;

	memcpy(list.type, chunk->cc, 4);
	list.size = chunk->size;

	read(fd, list.cc, 4);

	dx_avi_list_print(&list);

	pos = lseek(fd, 0, SEEK_CUR);
	eof_pos = pos + list.size;

	while(pos < eof_pos) {
		nread = file_avi_read_chunk(fd);
		if(nread < 0)
			break;
		pos += nread;
		lseek(fd, pos, SEEK_SET);
	}

	return DX_AVI_LIST_SIZE(chunk->size);
}

int dx_avi_chunk_handler(int fd, AVI_CHUNK* chunk) {
	dx_avi_list_print(chunk);

	return DX_AVI_CHUNK_SIZE(chunk->size);
}

int dx_avi_chunk_idx1_handler(int fd, AVI_CHUNK* chunk) {
	dx_avi_index_entry_t entry;
	int i = 0;
	off_t curpos;
	char chunkid[5];

	curpos = lseek(fd, 0, SEEK_CUR);

	while(sizeof(dx_avi_index_entry_t) * i < chunk->size && i < 100) {
		read(fd, &entry, sizeof(dx_avi_index_entry_t));
		memset(chunkid, 0x0, 5);
		memcpy(chunkid, &entry.ckid, 4);
		printf("Index [%d] %s %d %d %d\n", i, chunkid, entry.flags, entry.offset, entry.length);
		i++;
	}

	dx_avi_chunk_print(chunk);

	return DX_AVI_CHUNK_SIZE(chunk->size);
}
