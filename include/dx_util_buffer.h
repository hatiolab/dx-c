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

#ifndef __DX_UTIL_BUFFER_H
#define __DX_UTIL_BUFFER_H

#include <stdint.h>

typedef struct dx_buffer {
	int capacity;
	int position;
	int limit;
	int mark;
	uint8_t data[0];
} dx_buffer_t;

#define DX_BUFFER_SIZE(sz) (sizeof(dx_buffer_t)+sz)

dx_buffer_t* dx_buffer_alloc(int capacity);
void dx_buffer_free(dx_buffer_t* pbuf);
int dx_buffer_clear(dx_buffer_t* pbuf);
int dx_buffer_reset(dx_buffer_t* pbuf);
void dx_buffer_flip(dx_buffer_t* pbuf);
void dx_buffer_setpos(dx_buffer_t* pbuf, int pos);
int dx_buffer_getpos(dx_buffer_t* pbuf);
void dx_buffer_setlimit(dx_buffer_t* pbuf, int limit);
int dx_buffer_getlimit(dx_buffer_t* pbuf);
int dx_buffer_capacity(dx_buffer_t* pbuf);
int dx_buffer_remains(dx_buffer_t* pbuf);
int dx_buffer_put(dx_buffer_t* pbuf, void* psrc, int sz);
int dx_buffer_get(dx_buffer_t* pbuf, void* pdest, int sz);

#endif /* __DX_UTIL_BUFFER_H */
