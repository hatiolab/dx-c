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
#include <strings.h>	// For bzero
#include <sys/epoll.h>	// For EPOLLIN

#include "dx_debug_assert.h"
#include "dx_debug_malloc.h"

#include "dx_event_mplexer.h"
#include "dx_event_pipe.h"

int dx_event_pipe_handler(dx_event_context_t* context);

dx_event_pipe_context_t* dx_event_pipe_start() {
	int pipefds[2] = {};
	dx_event_context_t* pcontext;
	int flags;
	int ret;

	ret = pipe2(pipefds, O_NONBLOCK | O_CLOEXEC);

	ASSERT("Pipe Creating Error.\n", ret == 0);

	dx_event_pipe_context_t* epc = (dx_event_pipe_context_t*)MALLOC(sizeof(dx_event_context_t));

	epc->read_pipe = pipefds[0];
	epc->write_pipe = pipefds[1];

	// read-end를 non-blocking으로 만든다.
	flags = fcntl(epc->read_pipe, F_GETFL, 0);
	fcntl(epc->read_pipe, F_SETFL, flags | O_NONBLOCK);

	pcontext = dx_event_context_create();
	pcontext->fd = epc->read_pipe;
	pcontext->readable_handler = dx_event_pipe_handler;
	pcontext->writable_handler = NULL;
	pcontext->error_handler = NULL;

	dx_add_event_context(pcontext, EPOLLIN);

	return epc;
}

int dx_event_pipe_handler(dx_event_context_t* context) {

    char buf[128];

    bzero(buf, 128);

    ssize_t nbytes = read(context->fd, buf, sizeof(buf));
    if(0 == nbytes) {
        printf("Console hung up\n");
        return -1;
    } else if(0 > nbytes) {
        perror("Console read() error");
        close(context->fd);
        dx_del_event_context(context);
        return -2;
    }

    printf("[READ FROM PIPE] %s\n", buf);

    return 0;
}

int dx_send_to_pipe(dx_event_pipe_context_t* pcontext, int8_t* data, int sz) {
	int nwrite = write(pcontext->write_pipe, data, sz);
	return nwrite;
}
