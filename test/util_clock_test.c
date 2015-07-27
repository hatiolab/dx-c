#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // For sleep

#include "dx.h"

#include "dx_debug_assert.h"
#include "dx_debug_malloc.h"

#include "dx_util_log.h"

#include "dx_util_clock.h"
#include "dx_net.h"

void util_clock_test() {

	int i = 0;
	LONGLONG t_start = 0;
	LONGLONG t_end = 0;

	dx_clock_get_abs_msec(&t_start);
	t_start = htonll(t_start);
	t_start = ntohll(t_start);

	while(i++ < 100000) {
		dx_clock_get_abs_msec(&t_end);
		t_end = htonll(t_end);
		t_end = ntohll(t_end);
	}

	CONSOLE("Elapsed time for getting clock 100,000 time : %lld milli-seconds\n", t_end - t_start);

	CHKMEM();
}
