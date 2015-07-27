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

#ifndef __DX_EVENT_MPLEXER_H
#define __DX_EVENT_MPLEXER_H

#include <pthread.h>
#include <netinet/in.h>   // For uint32_t

#include "dx_util_buffer.h"
#include "dx_util_list.h"
#include "dx_util_clock.h"

/*
 * Definitions
 */

#define DX_MAX_EVENT_POLL_SIZE          100

#define DX_EVENT_MPLEXER_STATE_NOTHING      0
#define DX_EVENT_MPLEXER_STATE_POLLING      1

typedef struct dx_event_context dx_event_context_t;
typedef int (*dx_event_handler)(dx_event_context_t*);
typedef int (*dx_event_context_destroy_handler)(void*);

struct dx_event_context {
	int fd;

	dx_event_handler readable_handler;
	dx_event_handler writable_handler;
	dx_event_handler error_handler;

	/* 현재 읽기 진행중인 바이트 버퍼 */
	dx_buffer_t* pbuf_reading;

	/* 현재 쓰기 진행중인 바이트 버퍼 리스트 */
	dx_list_t* plist_writing;

	/* 사용자정의 이벤트 핸들러 - 커스터마이징 용도 */
	void* user_handler;
	/* 사용자정의 데이타 - 커스터마이징 용도 */
	void* pdata;
	/* 사용자 데이타 destroyer */

	LONGLONG last_clock_touch;
	dx_event_context_destroy_handler on_destroy;
};

struct dx_event_mplexer {
	int fd;
	int state;
	int signo;
	dx_list_t context_list;
	pthread_t polling_thread;
	struct epoll_event* events;
	int control_fd;
};

typedef struct dx_event_mplexer dx_event_mplexer_t;

int dx_event_mplexer_create();
int dx_event_mplexer_destroy();

int dx_event_mplexer_poll(int ts);
int dx_event_mplexer_wakeup();

dx_event_context_t* dx_event_context_create();
dx_event_context_t* dx_get_event_context(int fd);
int dx_add_event_context(struct dx_event_context* pcontext, uint32_t events);
int dx_mod_event_context(dx_event_context_t* context, uint32_t events);
int dx_del_event_context(dx_event_context_t* context);
int dx_clear_event_context();

void dx_event_context_touch(dx_event_context_t* context);

#endif /* __DX_EVENT_MPLEXER_H */
