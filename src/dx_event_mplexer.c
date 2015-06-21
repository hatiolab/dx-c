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

#include "../include/dx_event_mplexer.h"

#include <stdio.h>
#include <stdlib.h>			// For exit, ..
#include <sys/epoll.h>		// For epoll
#include <unistd.h>			// For close

#include "../include/dx_debug_assert.h"
#include "../include/dx_util_list.h"

dx_event_mplexer_t* __dx_mplexer;

int dx_event_context_find(void* context1, void* context2) {
	return ((dx_event_context_t*)context1)->fd - ((dx_event_context_t*)context2)->fd;
}

int dx_event_context_destroyer(void* data) {
	dx_event_context_t* context = (dx_event_context_t*)data;

	epoll_ctl(__dx_mplexer->fd, EPOLL_CTL_DEL, context->fd, NULL);

	FREE(context);

	return 0;
}

int dx_event_mplexer_create() {

	if(__dx_mplexer)
		return 0; // Already exist.

	__dx_mplexer = (dx_event_mplexer_t*)MALLOC(sizeof(dx_event_mplexer_t));

	dx_list_init(&__dx_mplexer->context_list, dx_event_context_find, dx_event_context_destroyer);

	__dx_mplexer->fd = epoll_create1(EPOLL_CLOEXEC);
	if(__dx_mplexer->fd < 0) {
		perror("Multiplexer - epoll_create() error");
		return -1;
	}

	__dx_mplexer->events = (struct epoll_event*)MALLOC(sizeof(struct epoll_event) * DX_MAX_EVENT_POLL_SIZE);

	return 0;
}

int dx_event_mplexer_destroy() {
	dx_clear_event_context();
	dx_list_clear(&__dx_mplexer->context_list);

	FREE(__dx_mplexer->events);

	close(__dx_mplexer->fd);
	__dx_mplexer->fd = 0;

	FREE(__dx_mplexer);
	__dx_mplexer = NULL;

	return 0;
}

int dx_event_mplexer_poll() {
	int i, n;

	__dx_mplexer->state = 0;

	n = epoll_wait(__dx_mplexer->fd, __dx_mplexer->events, DX_MAX_EVENT_POLL_SIZE, -1);
	if(-1 == n) {
		perror("Multiplexer - epoll_wait() error");
		exit(1);
	}

	for(i = 0;i < n;i++) {
		struct dx_event_context *context = (struct dx_event_context*)__dx_mplexer->events[i].data.ptr;
		uint32_t flags = __dx_mplexer->events[i].events;

		if((flags & EPOLLIN) && context->readable_handler) {
			context->readable_handler(context);
		}
		if((flags & EPOLLOUT) && context->writable_handler) {
			context->writable_handler(context);
		}
		if((flags & EPOLLERR) && context->error_handler) {
			context->error_handler(context);
		}
	}

	return __dx_mplexer->state;
}

int dx_event_mplexer_kill(int signo) {
	__dx_mplexer->state = signo;

	return 0;
}

int dx_add_event_context(int fd, uint32_t events, dx_event_handler readable_handler, dx_event_handler writable_handler, dx_event_handler error_handler) {
	struct dx_event_context* context;
	struct epoll_event event;

	context = MALLOC(sizeof(struct dx_event_context));

	context->fd = fd;

	context->readable_handler = readable_handler;
	context->writable_handler = writable_handler;
	context->error_handler = error_handler;

	dx_list_add(&__dx_mplexer->context_list, context);

	event.events = events;
	event.data.ptr = context;
	epoll_ctl(__dx_mplexer->fd, EPOLL_CTL_ADD, fd, &event);

	return 0;
}

int dx_mdx_event_context(struct dx_event_context* context, uint32_t events) {
	struct epoll_event event;

	event.events = events;
	event.data.ptr = context;
	if(0 > epoll_ctl(__dx_mplexer->fd, EPOLL_CTL_MOD, context->fd, &event)) {
		printf("Multiplexer - epoll_ctl (EPOLL_CTL_MOD) error.\n");
	}

	return 0;
}

int dx_del_event_context(struct dx_event_context* context) {
	dx_list_remove(&__dx_mplexer->context_list, context);

	return 0;
}

int dx_clear_event_context() {
	dx_list_clear(&__dx_mplexer->context_list);

	return 0;
}

