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

#ifndef __DX_UTIL_BUFFER_H
#define __DX_UTIL_BUFFER_H

#include <stdint.h>

typedef struct dx_buffer {
  int capacity;
  int position;
  int limit;
  int mark;
  int8_t* data;
} dx_buffer_t;

dx_buffer_t* dx_buffer_alloc(int capacity);
dx_buffer_t* dx_buffer_wrap(int8_t* buffer, int size);
void dx_buffer_free(dx_buffer_t* pbuf);
void dx_buffer_clear(dx_buffer_t* pbuf);
void dx_buffer_reset(dx_buffer_t* pbuf);
void dx_buffer_flip(dx_buffer_t* pbuf);
void dx_buffer_setpos(dx_buffer_t* pbuf, int pos);
int dx_buffer_getpos(dx_buffer_t* pbuf);
/*
 * dx_buffer_step_forward
 *
 * 바이트 버퍼의 현재 포지션을 steps 만큼 앞으로 이동시킨다.
 */
void dx_buffer_step_forward(dx_buffer_t* pbuf, int steps);
void dx_buffer_setlimit(dx_buffer_t* pbuf, int limit);
int dx_buffer_getlimit(dx_buffer_t* pbuf);
int dx_buffer_capacity(dx_buffer_t* pbuf);
int dx_buffer_remains(dx_buffer_t* pbuf);
/*
 * dx_buffer_ppos
 *
 * 바이트 버퍼의 현재 포지션에 해당하는 주소를 반환한다.
 */
int8_t* dx_buffer_ppos(dx_buffer_t* pbuf);
int dx_buffer_put(dx_buffer_t* pbuf, void* psrc, int sz);
int dx_buffer_get(dx_buffer_t* pbuf, void* pdest, int sz);

/*
 * dx_buffer_read_from
 *
 * 파일로부터 버퍼로 읽어온다.
 */
int dx_buffer_read_from(dx_buffer_t* pbuf, int fd);
int dx_buffer_nread_from(dx_buffer_t* pbuf, int sz, int fd);

#endif /* __DX_UTIL_BUFFER_H */
