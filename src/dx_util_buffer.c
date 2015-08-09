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

#include <string.h> // For memcpy
#include <unistd.h> // For read

#include "dx.h"

#include "dx_debug_assert.h"
#include "dx_debug_malloc.h"

#include "dx_util_buffer.h"

/*
 * TODO 버퍼를 외부에서 제공하는 방법도 제공해야 한다.
 */
dx_buffer_t* dx_buffer_alloc(int capacity) {
	dx_buffer_t* pbuf = (dx_buffer_t*) MALLOC(sizeof(dx_buffer_t));

	pbuf->data = MALLOC(capacity);
	pbuf->capacity = capacity;
	dx_buffer_clear(pbuf);
	return pbuf;
}

dx_buffer_t* dx_buffer_wrap(int8_t* buffer, int size) {
	dx_buffer_t* pbuf = (dx_buffer_t*) MALLOC(sizeof(dx_buffer_t));

	pbuf->data = buffer;
	pbuf->capacity = size;
	dx_buffer_clear(pbuf);

	return pbuf;
}

void dx_buffer_free(dx_buffer_t* pbuf) {
	FREE(pbuf->data);
	FREE(pbuf);
}

void dx_buffer_clear(dx_buffer_t* pbuf) {
	pbuf->position = 0;
	pbuf->limit = pbuf->capacity;
	pbuf->mark = -1;
}

void dx_buffer_reset(dx_buffer_t* pbuf) {
	ASSERT("Buffer Mark should not be negative value.", pbuf->mark >= 0);
	pbuf->position = pbuf->mark;
}

void dx_buffer_flip(dx_buffer_t* pbuf) {
	pbuf->limit = pbuf->position;
	pbuf->position = 0;
}

void dx_buffer_setpos(dx_buffer_t* pbuf, int pos) {
	pbuf->position = pos;
}

int dx_buffer_getpos(dx_buffer_t* pbuf) {
	return pbuf->position;
}

void dx_buffer_step_forward(dx_buffer_t* pbuf, int steps) {
	pbuf->position += steps;
	ASSERT("Buffer position should not greater than limit", pbuf->limit >= pbuf->position);
}

void dx_buffer_setlimit(dx_buffer_t* pbuf, int limit) {
	ASSERT("Buffer Limit should not greater than capacity", limit <= pbuf->capacity);
	pbuf->limit = limit;
}

int dx_buffer_getlimit(dx_buffer_t* pbuf) {
	return pbuf->limit;
}

int dx_buffer_capacity(dx_buffer_t* pbuf) {
	return pbuf->capacity;
}

int dx_buffer_remains(dx_buffer_t* pbuf) {
	return pbuf->limit - pbuf->position;
}

int8_t* dx_buffer_ppos(dx_buffer_t* pbuf) {
	return &(pbuf->data[0]) + pbuf->position;
}

int dx_buffer_put(dx_buffer_t* pbuf, void* psrc, int sz) {
	int remains = pbuf->limit - pbuf->position;
	int copied = sz > remains ? remains : sz;

	memcpy(&pbuf->data[pbuf->position], psrc, copied);

	pbuf->position += copied;

	return copied;
}

int dx_buffer_get(dx_buffer_t* pbuf, void* pdest, int sz) {
	int remains = pbuf->limit - pbuf->position;
	int copied = sz > remains ? remains : sz;

	memcpy(pdest, &pbuf->data[pbuf->position], copied);

	pbuf->position += copied;

	return copied;
}

int dx_buffer_read_from(dx_buffer_t* pbuf, int fd) {
	int nread = dx_buffer_remains(pbuf);

	return dx_buffer_nread_from(pbuf, nread, fd);
}

int dx_buffer_nread_from(dx_buffer_t* pbuf, int sz, int fd) {
	int nread = read(fd, dx_buffer_ppos(pbuf), sz);
	if(nread <= 0)
		return nread;

	dx_buffer_step_forward(pbuf, nread);

	return nread;
}

