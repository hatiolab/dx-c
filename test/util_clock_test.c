#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // For sleep

#include "dx_debug_assert.h"
#include "dx_debug_malloc.h"

#include "dx_util_log.h"

#include "dx_util_clock.h"

void util_clock_test() {

	int i = 0;
	LONGLONG t = 0;

	while(i++ < 10) {

		dx_clock_get_abs_msec(&t);
		CONSOLE("Clock(msec) now : %lld\n", t);
		sleep(1);
	}

	CHKMEM();
}
