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

#define _GNU_SOURCE

#include <unistd.h>		// For pipe
#include <fcntl.h>		// For fcntl
#include <stddef.h>		// For NULL
#include <sys/epoll.h>	// For EPOLLIN

#include "dx.h"

#include "dx_debug_assert.h"
#include "dx_debug_malloc.h"

#include "dx_event_mplexer.h"
#include "dx_event_pipe.h"

int dx_event_control_handler(dx_event_context_t* pcontext);

int dx_event_control_start(int* peer) {
	return dx_event_pipe_start(dx_event_control_handler, peer);
}

int dx_event_control_handler(dx_event_context_t* pcontext) {
	uint8_t ch;

	ssize_t nbytes = read(pcontext->fd, &ch, 1);
	if (0 == nbytes) {
		printf("Event control hung up\n");
		return -1;
	} else if (0 > nbytes) {
		perror("Event control  read() error");
		close(pcontext->fd);
		dx_del_event_context(pcontext);
		return -2;
	}

	/* Do nothing. just wake up event multiplexer */

	return 0;
}
