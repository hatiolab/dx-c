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

#define _GNU_SOURCE

#include <unistd.h>		// For pipe
#include <fcntl.h>		// For fcntl
#include <stddef.h>		// For NULL
#include <sys/epoll.h>	// For EPOLLIN

#include "dx_debug_assert.h"
#include "dx_debug_malloc.h"

#include "dx_event_mplexer.h"
#include "dx_net_pipe.h"

dx_pipe_context_t* dx_event_pipe_start(dx_event_handler handler) {
	int pipefds[2] = {};
	dx_event_context_t* pcontext;
	int flags;
	int ret;

	ret = pipe2(pipefds, O_NONBLOCK | O_CLOEXEC);

	ASSERT("Pipe Creating Error.\n", ret == 0);

	dx_pipe_context_t* epc = (dx_pipe_context_t*)MALLOC(sizeof(dx_event_context_t));

	epc->read_pipe = pipefds[0];
	epc->write_pipe = pipefds[1];

	// read-end를 non-blocking으로 만든다.
	flags = fcntl(epc->read_pipe, F_GETFL, 0);
	fcntl(epc->read_pipe, F_SETFL, flags | O_NONBLOCK);

	pcontext = dx_event_context_create();
	pcontext->fd = epc->read_pipe;
	pcontext->readable_handler = handler;
	pcontext->writable_handler = NULL;
	pcontext->error_handler = NULL;

	dx_add_event_context(pcontext, EPOLLIN);

	return epc;
}

int dx_send_to_pipe(dx_pipe_context_t* pcontext, int8_t* data, int sz) {
	int nwrite = write(pcontext->write_pipe, data, sz);
	return nwrite;
}
