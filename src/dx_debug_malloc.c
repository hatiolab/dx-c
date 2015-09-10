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

#include <stddef.h>
#include <stdint.h> // For uint8_t
#include <stdlib.h> // For malloc, free, size_t
#include <string.h> // For memset

#include "dx.h"

#include "dx_debug_assert.h"

#include "dx_util_log.h"
#include "dx_util_list.h"
#include "dx_util_lock.h"

#ifdef DX_DEBUG

#define DX_MALLOC_WATERMARK "DXMAWTMR"
#define DX_MALLOC_WATERMARK_SIZE 8

typedef struct dx_malloc_head {
	char watermark[DX_MALLOC_WATERMARK_SIZE];
	char filename[48];
	int line;
	int size;
} dx_malloc_head_t;

typedef struct dx_malloc {
	dx_malloc_head_t head;
	uint8_t allocated[0];
}__attribute__((__packed__)) dx_malloc_t;

#define DX_MALLOC_HEAD_SIZE (sizeof(dx_malloc_head_t))

/*
 * 메모리 할당 및 해제 횟수를 유지함.
 * 성능 하락을 방지하기 위해서 Lock 은 적용하지 않으므로, 정확성은 신뢰할 수 없음.
 */
long __dx_alloc_count = 0;
long __dx_free_count = 0;

/*
 * 메모리 할당된 아이템들의 리스트를 유지함.
 * 성능 하락을 방지하기 위해서 Lock 은 적용하지 않으므로, 정확성은 신뢰할 수 없음.
 */
dx_list_t __dx_alloc_list;

/*
 * 무한 Loop를 방지하기 위해서 사용되는 Flag
 */
int __dx_alloc_except_flag = 0;

void dx_malloc_set_except_flag(int flag) {
	__dx_alloc_except_flag = flag;
}

/*
 * 메모리 할당과 해제를 검증하기 위한 몇가지 작업을 포함한 메모리 할당
 * 1. 메모리 할당 관련 워터마크
 * 2. 메모리가 할당된 소스의 위치를 기록
 */
void* dx_malloc(size_t sz, char* fname, int line) {
	void* p;
	dx_malloc_head_t* head;

	DX_LOCK(&__dx_alloc_list.mutex);

	if (__dx_alloc_except_flag) {
		DX_UNLOCK(&__dx_alloc_list.mutex);
		return malloc(sz);
	}

	DX_UNLOCK(&__dx_alloc_list.mutex);

	p = malloc(sz + DX_MALLOC_HEAD_SIZE);
	ASSERT("Memory allocation failed.", p != NULL)
	if(p == NULL)
		return NULL;

	head = (dx_malloc_head_t*) p;

	if (__dx_alloc_count == 0)
		dx_list_init(&__dx_alloc_list, NULL, NULL);

	memset(head, 0, DX_MALLOC_HEAD_SIZE);
	strncpy(head->watermark, DX_MALLOC_WATERMARK, DX_MALLOC_WATERMARK_SIZE);
	strncpy(head->filename, fname, sizeof(head->filename));
	head->line = line;
	head->size = sz;

	__dx_alloc_count++;

	DX_LOCK(&__dx_alloc_list.mutex);

	dx_malloc_set_except_flag(1);
	dx_list_add(&__dx_alloc_list, p);
	dx_malloc_set_except_flag(0);

	DX_UNLOCK(&__dx_alloc_list.mutex);

	return (void*) &(((dx_malloc_t*) head)->allocated[0]);
}

/*
 * 메모리 해제 관련 검증을 하는 함수임.
 * 1. 이미 해제된 메모리를 해제하지는 않는가 ?
 * 2. 정상적으로 할당된 메모리를 해제하고 있는가 ?
 *
 */
void dx_free(void* p, char* filename, int line) {
	DX_LOCK(&__dx_alloc_list.mutex);

	if (__dx_alloc_except_flag) {
		DX_UNLOCK(&__dx_alloc_list.mutex);
		free(p);
		return;
	}

	DX_UNLOCK(&__dx_alloc_list.mutex);

	dx_malloc_head_t* head = (dx_malloc_head_t*) (p - DX_MALLOC_HEAD_SIZE);
	if (0 != memcmp(head->watermark, DX_MALLOC_WATERMARK,
	DX_MALLOC_WATERMARK_SIZE)) {
		CONSOLE("[ASSERT FREE] allocated at %20s:%d(sized %d), freed at %s:%d", head->filename, head->line, head->size, filename, line);
		dx_trace();
		exit(0);
	}

	memset(head->watermark, 0x0, DX_MALLOC_WATERMARK_SIZE);

	__dx_free_count++;

	DX_LOCK(&__dx_alloc_list.mutex);

	dx_malloc_set_except_flag(1);
	dx_list_remove(&__dx_alloc_list, head);
	dx_malloc_set_except_flag(0);

	DX_UNLOCK(&__dx_alloc_list.mutex);

	free(head);
}

void dx_chkmem_callback(dx_malloc_head_t* p) {
	CONSOLE("[MEM] allocated on %48s:%d(size %d)\n", p->filename, p->line, p->size);
}

void dx_chkmem() {
	CONSOLE("[CHKMEM] %ld Allocated, %ld Freed. \n", __dx_alloc_count, __dx_free_count);

	dx_list_iterator(&__dx_alloc_list, (dx_list_iterator_callback) dx_chkmem_callback);
	dx_list_clear(&__dx_alloc_list);

	__dx_alloc_count = 0;
	__dx_free_count = 0;
}

#endif
