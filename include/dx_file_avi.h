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
	uint32_t	ckid; /* chunk id */
	uint32_t	flags;
	uint32_t	offset;
	uint32_t	length;
} dx_avi_index_entry_t;

#define DX_AVI_CHUNK_SIZE(sz) (sizeof(dx_avi_chunk_t) + sz)
#define DX_AVI_LIST_SIZE(sz) (sizeof(dx_avi_list_t) + sz - 4)

int dx_avi_info(char* path);
int dx_avi_open(char* path);
int dx_avi_is_valid_chunk(dx_avi_chunk_t* chunk);
int dx_avi_find_index_chunk(int fd, dx_avi_chunk_t* chunk);
int dx_avi_chunk_idx1_handler(int fd, dx_avi_chunk_t* chunk);

void dx_avi_chunk_print(dx_avi_chunk_t* chunk);
void dx_avi_list_print(dx_avi_list_t* list);
void dx_avi_index_print(dx_avi_index_entry_t* index);

#endif /* __DX_FILE_AVI_H */
