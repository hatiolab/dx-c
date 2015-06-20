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

#ifndef __DX_EVENT_MPLEXER_H
#define __DX_EVENT_MPLEXER_H

#include <netinet/in.h>		// For uint32_t

#include "dx_util_list.h"

/*
 * Definitions
 */

#define DX_MAX_EVENT_POLL_SIZE					20

typedef struct dx_event_context dx_event_context_t;
typedef int (*dx_event_handler)(dx_event_context_t*);

struct dx_event_context {
    int         fd;

    dx_event_handler readable_handler;
    dx_event_handler writable_handler;
    dx_event_handler error_handler;
};

struct dx_event_mplexer {
	int	fd;
	int state;
	dx_list_t	context_list;
	struct epoll_event* events;
};

typedef struct dx_event_mplexer dx_event_mplexer_t;

int dx_event_mplexer_create();
int dx_event_mplexer_destroy();

int dx_event_mplexer_poll();
int dx_event_mplexer_kill(int signo);

int dx_add_event_context(int fd, uint32_t events, dx_event_handler readable_handler, dx_event_handler writable_handler, dx_event_handler error_handler);
int dx_mdx_event_context(dx_event_context_t* context, uint32_t events);
int dx_del_event_context(dx_event_context_t* context);
int dx_clear_event_context();

#endif /* __DX_EVENT_MPLEXER_H */
