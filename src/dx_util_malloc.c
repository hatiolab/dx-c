// Copyright (c) 2015 - 2015 All Right Reserved, http://hatiolab.com
//
// This source is subject to the ImageNext Permissive License.
// Please see the License.txt file for more information.
// All other rights reserved.
//
// THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WIHOUT WARRANTY OF ANY
// KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//

#include <stdint.h> // For uint8_t
#include <stdlib.h> // For malloc, free, size_t

#include "../include/dx_assert.h"

#define DX_MALLOC_WATERMARK "DXMA"
#define DX_MALLOC_WATERMARK_FREE "DXFR"
#define DX_MALLOC_WATERMARK_SIZE 4

typedef struct dx_malloc_head {
	char watermark[4];
	char filename[52];
	int line;
	int size;
} dx_malloc_head_t;

typedef struct dx_malloc {
	dx_malloc_head_t head;
	uint8_t allocated[0];
} __attribute__((__packed__)) dx_malloc_t;

#define DX_MALLOC_HEAD_SIZE (sizeof(dx_malloc_head_t))

/*
 * 메모리 할당과 해제를 검증하기 위한 몇가지 작업을 포함한 메모리 할당
 * 1. 메모리 할당 관련 워터마크
 * 2. 메모리가 할당된 소스의 위치를 기록
 */
void* dx_malloc(size_t sz, char* fname, int line) {
	void* p = malloc(sz + DX_MALLOC_HEAD_SIZE);
	dx_malloc_head_t* head = (dx_malloc_head_t*)p;

	memset(head, 0, DX_MALLOC_HEAD_SIZE);
	strncpy(head->watermark, DX_MALLOC_WATERMARK, DX_MALLOC_WATERMARK_SIZE);
	strncpy(head->filename, fname, sizeof(head->filename));
	head->line = line;
	head->size = sz;

	return (void*)&(((dx_malloc_t*)head)->allocated[0]);
}

/*
 * 메모리 해제 관련 검증을 하는 함수임.
 * 1. 이미 해제된 메모리를 해제하지는 않는가 ?
 * 2. 정상적으로 할당된 메모리를 해제하고 있는가 ?
 *
 */
void dx_free(void* p, char* filename, int line) {
	dx_malloc_head_t* head = (dx_malloc_head_t*)(p - DX_MALLOC_HEAD_SIZE);
	if(0 != strncmp(head->watermark, DX_MALLOC_WATERMARK, DX_MALLOC_WATERMARK_SIZE)) {
		printf("[ASSERT FREE] allocated at %20s:%d(sized %d), freed at %s:%d", head->filename, head->line, head->size, filename, line);
		exit(0);
	}

	memset(head->watermark, 0x0, DX_MALLOC_WATERMARK_SIZE);

	free(head);
}

void dx_chkmem() {
	printf("Checked.\n");
}
