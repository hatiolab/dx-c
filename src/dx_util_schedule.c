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

#include <stdio.h>
#include <stdlib.h>
#include <string.h> // For memcpy
#include <unistd.h> // For read
#include <stdarg.h>	// For va_start, va_end, va_arg,..
#include <sys/sysinfo.h>

#include "dx.h"

#include "dx_debug_malloc.h"
#include "dx_debug_assert.h"

#include "dx_util_log.h"
#include "dx_util_list.h"
#include "dx_util_schedule.h"
#include "dx_util_clock.h"

dx_list_t* __dx_schedule_list = NULL;

int dx_scheduler_destroyer(void* data) {
	FREE(data);
	return 0;
}

void dx_scheduler_start() {
	ASSERT("Schedule System should not be started.", __dx_schedule_list == NULL)

	__dx_schedule_list = (dx_list_t*)MALLOC(sizeof(dx_list_t));
	dx_list_init(__dx_schedule_list, NULL, dx_scheduler_destroyer);
}

void dx_scheduler_stop() {
	ASSERT("Schedule System should be already started.", __dx_schedule_list != NULL)

	dx_list_close(__dx_schedule_list);
	FREE(__dx_schedule_list);
	__dx_schedule_list = NULL;
}

void dx_scheduler_next_callback(void* ps, va_list ap) {
	dx_schedule_t* schedule = (dx_schedule_t*)ps;

	LONGLONG* next = va_arg(ap, LONGLONG*);

	if(schedule->next_schedule <= 0)
		return;

	if(*next == 0 || schedule->next_schedule < *next)
		*next = schedule->next_schedule;
}

long dx_scheduler_next_wait() {
	LONGLONG next, now;

	dx_clock_get_abs_msec(&now);
	next = 0;

	dx_list_iterator(__dx_schedule_list, dx_scheduler_next_callback, &next, now);

	if(next == 0)
		return -1;

	return (next - now) > 0 ? next - now : 0;
}

void dx_scheduler_do_callback(void* ps, va_list ap) {
	dx_schedule_t* schedule = (dx_schedule_t*)ps;

	LONGLONG now;

	now = va_arg(ap, LONGLONG);

	if(schedule->next_schedule > 0 && schedule->next_schedule <= now) {
		schedule->callback(schedule->clojure);
		if(schedule->repeatable != 0 && schedule->next_schedule > 0)
			schedule->next_schedule += schedule->interval;
		else
			schedule->next_schedule = 0;
	}
}

void dx_scheduler_cancel_callback(void* ps, va_list ap) {
	dx_schedule_t* schedule = (dx_schedule_t*)ps;

	dx_list_t* cancel_list = va_arg(ap, dx_list_t*);

	if(schedule->next_schedule == 0)
		dx_list_add(cancel_list, schedule);
}

void dx_scheduler_delete_callback(void* ps, va_list ap) {
	dx_list_t* list = va_arg(ap, dx_list_t*);

	dx_list_remove(list, (dx_schedule_t*)ps);
}

void dx_scheduler_do() {
	dx_list_t cancel_list;
	LONGLONG now;

	dx_clock_get_abs_msec(&now);

	dx_list_init(&cancel_list, NULL, NULL);

	/* 스케쥴에 해당하는 태스크들을 수행한다. */
	dx_list_iterator(__dx_schedule_list, dx_scheduler_do_callback, now);

	/* 스케쥴이 끝난 태스크들을 수집한다. */
	dx_list_iterator(__dx_schedule_list, dx_scheduler_cancel_callback, &cancel_list);

	/* 스케쥴이 끝난 태스크들을 제거한다. - concurrency 문제 때문에, 이렇게 수행한다. */
	dx_list_iterator(&cancel_list, dx_scheduler_delete_callback, &cancel_list);

	/* 취소된 스케쥴 리스트를 모두 삭제한다. */
	dx_list_clear(&cancel_list);
}

dx_schedule_t* dx_schedule_register(long initial_interval, long interval,
		int repeatable, dx_schedule_callback callback, void* clojure) {
	LONGLONG now;

	dx_clock_get_abs_msec(&now);

	dx_schedule_t* schedule = (dx_schedule_t*)MALLOC(sizeof(dx_schedule_t));

	schedule->callback = callback;
	schedule->clojure = clojure;
	schedule->interval = interval;
	schedule->repeatable = repeatable;
	schedule->next_schedule = now + initial_interval;

	dx_list_add(__dx_schedule_list, schedule);

	return schedule;
}

void dx_schedule_cancel(dx_schedule_t* schedule) {
	schedule->next_schedule = 0;
}

long dx_schedule_next(dx_schedule_t* schedule) {
	return schedule->next_schedule;
}
