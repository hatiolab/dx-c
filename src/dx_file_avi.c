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
	{ NULL },
};

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

int dx_avi_find_index_chunk(int fd, AVI_CHUNK* chunk) {
	off_t offset;
	int i = 0;
	int nread;
	AVI_LIST head;
	AVI_CHUNK tmp;

	read(fd, &head, sizeof(AVI_LIST));
	offset = lseek(fd, sizeof(AVI_LIST), SEEK_SET);

	while(offset < DX_AVI_LIST_SIZE(head.size)) {
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

	demo_print_chunk(chunk);

	return DX_AVI_CHUNK_SIZE(chunk->size);
}
