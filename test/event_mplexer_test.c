#include <stdio.h>
#include <stdlib.h>

#include "dx.h"

#include "dx_debug_assert.h"
#include "dx_debug_malloc.h"

#include "dx_util_buffer.h"

#include "dx_event_mplexer.h"

void event_mplexer_test() {
	dx_event_mplexer_create();

	dx_event_mplexer_destroy();

	CHKMEM();
}
