// Copyright (c) 2015 - 2015 All Right Reserved, http://hatiolab.com
//
// This source is subject to the ImageNext Permissive License.
// Please see the License.txt file for more information.
// All other rights reserved.
//
// THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WIHOUT WARRANTY OF ANY
// KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//

#include <pthread.h>

#include "../include/dx_debug_assert.h"

pthread_mutex_t __dx_lock;

void dx_lock_init() {
	int success = pthread_mutex_init(&__dx_lock, NULL);
	ASSERT("Mutex Init Failed.\n", success == 0);
}

void dx_lock_destroy() {
	pthread_mutex_destroy(&__dx_lock);
}

void dx_lock() {
	pthread_mutex_lock(&__dx_lock);
}

void dx_unlock() {
	pthread_mutex_unlock(&__dx_lock);
}
