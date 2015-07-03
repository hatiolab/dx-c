#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // For sleep

#include "dx.h"

#include "dx_debug_assert.h"
#include "dx_debug_malloc.h"

#include "dx_util_log.h"
#include "dx_util_clock.h"
#include "dx_util_schedule.h"

int text_schedule_count = 0;

int test_schedule_callback(void* p) {
	CONSOLE("Schedule Callback %d\n", (int)p)

	text_schedule_count++;

	return 0;
}

void util_schedule_test() {

	int i = 0;

	dx_scheduler_start();

	for(i = 0;i < 10;i++)
		dx_schedule_register(i * 1000, 1000, 1/* repeatable */, test_schedule_callback, (void*)i);

	while(text_schedule_count < 100) {
		sleep(dx_scheduler_next_wait() / 1000);
		dx_scheduler_do();
	}

	dx_scheduler_stop();

	CHKMEM();
}
