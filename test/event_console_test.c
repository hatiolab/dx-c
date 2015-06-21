#include <stdio.h>
#include <stdlib.h>

#include "dx_debug_assert.h"
#include "dx_debug_malloc.h"

#include "dx_util_buffer.h"

#include "dx_event_mplexer.h"
#include "dx_console.h"

void event_console_test() {
	dx_event_mplexer_create();

	dx_console_start();

	dx_event_mplexer_poll();

	CHKMEM();
}
