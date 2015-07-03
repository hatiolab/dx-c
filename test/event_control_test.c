#include <stdio.h>
#include <stdlib.h>
#include <strings.h>	// For bzero
#include <fcntl.h>		// For fcntl
#include <unistd.h>		// For pipe
#include <stddef.h>		// For NULL
#include <sys/epoll.h>	// For EPOLLIN
#include <pthread.h>

#include "dx.h"

#include "dx_debug_assert.h"
#include "dx_debug_malloc.h"

#include "dx_util_buffer.h"

#include "dx_event_mplexer.h"

void* event_control_tester(void*);

void event_control_test() {
	pthread_t pid;

	/* event multiplexer start */
	dx_event_mplexer_create();

	pthread_create(&pid, NULL, event_control_tester, NULL);

	/* Big Loop */
	dx_event_mplexer_poll(-1);

	dx_event_mplexer_destroy();

	pthread_join(pid, NULL);

	CHKMEM();
}

void* event_control_tester(void* vague) {
	sleep(1);
	dx_event_mplexer_wakeup();

	return NULL;
}
