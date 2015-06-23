#include <stdio.h>
#include <stdlib.h>
#include <strings.h>	// For bzero
#include <fcntl.h>		// For fcntl
#include <unistd.h>		// For pipe
#include <stddef.h>		// For NULL
#include <sys/epoll.h>	// For EPOLLIN

#include "dx_debug_assert.h"
#include "dx_debug_malloc.h"

#include "dx_util_buffer.h"

#include "dx_event_mplexer.h"
#include "dx_event_pipe.h"

int dx_net_pipe_handler(dx_event_context_t* context);

void net_pipe_test() {
	int peer;

	dx_event_mplexer_create();

	dx_event_pipe_start(dx_net_pipe_handler, &peer);

	dx_event_mplexer_poll(1000);

	dx_event_mplexer_destroy();

	CHKMEM();
}

int dx_net_pipe_handler(dx_event_context_t* context) {

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
