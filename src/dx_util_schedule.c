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

#include <string.h> // For memcpy
#include <unistd.h> // For read
#include <sys/sysinfo.h>

#include "dx_debug_malloc.h"
#include "dx_debug_assert.h"

#include "dx_util_list.h"
#include "dx_util_schedule.h"

#include <stdio.h>
#include <stdlib.h>

dx_list_t* __dx_schedule_list = NULL;

void dx_scheduler_start() {
	ASSERT("Schedule System should not be started.", __dx_schedule_list == NULL)

	dx_list_init(__dx_schedule_list, NULL, NULL);
}

void dx_scheduler_stop() {
	ASSERT("Schedule System should be already started.", __dx_schedule_list != NULL)

	dx_list_clear(__dx_schedule_list);

	FREE(__dx_schedule_list);
}

long dx_scheduler_next() {
	return 0;
}

dx_schedule_t* dx_schedule_register(long initial_interval, long interval,
		int repeatable, dx_schedule_callback callback, void* clojure) {
	struct sysinfo si;

	sysinfo(&si);

	dx_schedule_t* schedule = (dx_schedule_t*)MALLOC(sizeof(dx_schedule_t));

	schedule->callback = callback;
	schedule->closure = clojure;
	schedule->interval = interval;
	schedule->repeatable = repeatable;
	schedule->next_schedule = si.uptime + initial_interval;

	dx_list_add(__dx_schedule_list, schedule);

	return schedule;
}

void dx_schedule_cancel(dx_schedule_t* schedule) {

}

long dx_schedule_next(dx_schedule_t* schedule) {
	return 0;
}
