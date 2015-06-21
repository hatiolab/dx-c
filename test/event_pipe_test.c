#include <stdio.h>
#include <stdlib.h>

#include "dx_debug_assert.h"
#include "dx_debug_malloc.h"

#include "dx_util_buffer.h"

#include "dx_event_mplexer.h"
#include "dx_event_pipe.h"

void event_pipe_test() {
	dx_event_mplexer_create();

	dx_event_pipe_start();

	dx_event_mplexer_poll();

	CHKMEM();
}
