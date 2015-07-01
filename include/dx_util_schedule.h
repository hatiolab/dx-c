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

#ifndef __DX_UTIL_SCHEDULE_H
#define __DX_UTIL_SCHEDULE_H

#include "dx_util_clock.h"

typedef int (*dx_schedule_callback)(void*);

typedef struct dx_schedule {
	long	interval;
	int		repeatable;
	LONGLONG 	next_schedule;
	dx_schedule_callback callback;
	void* clojure;
} dx_schedule_t;

void dx_scheduler_start();
void dx_scheduler_stop();
long dx_scheduler_next_wait();
void dx_scheduler_do();

dx_schedule_t* dx_schedule_create(long initial_interval, long interval,
		int repeatable, dx_schedule_callback callback, void* clojure);
dx_schedule_t* dx_schedule_register(long initial_interval, long interval,
		int repeatable, dx_schedule_callback callback, void* clojure);
void dx_schedule_cancel(dx_schedule_t* schedule);
long dx_schedule_next(dx_schedule_t* schedule);

#endif /* __DX_UTIL_SCHEDULE_H */
