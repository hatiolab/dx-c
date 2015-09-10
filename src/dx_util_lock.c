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

#include <pthread.h>

#include "dx.h"

#include "dx_debug_assert.h"
#include "dx_util_lock.h"

#ifdef DX_MULTITHREADED

pthread_mutex_t __dx_lock;
pthread_mutexattr_t __dx_lock_attr;

void dx_lock_global_init() {
	pthread_mutexattr_init(&__dx_lock_attr);
	pthread_mutexattr_settype(&__dx_lock_attr, PTHREAD_MUTEX_RECURSIVE);
	int success = pthread_mutex_init(&__dx_lock, &__dx_lock_attr);
	ASSERT("Mutex Init Failed.\n", success == 0);
}

void dx_lock_global_destroy() {
	pthread_mutex_destroy(&__dx_lock);
}

void dx_lock_global() {
	pthread_mutex_lock(&__dx_lock);
}

void dx_unlock_global() {
	pthread_mutex_unlock(&__dx_lock);
}

void dx_lock_init(pthread_mutex_t* mutex, pthread_mutexattr_t* attr) {
	pthread_mutexattr_init(attr);
	pthread_mutexattr_settype(attr, PTHREAD_MUTEX_RECURSIVE);
	int success = pthread_mutex_init(mutex, attr);
	ASSERT("Mutex Init Failed.\n", success == 0);
}

void dx_lock_destroy(pthread_mutex_t* mutex) {
	pthread_mutex_destroy(mutex);
}

void dx_lock(pthread_mutex_t* mutex) {
	pthread_mutex_lock(mutex);
}

void dx_unlock(pthread_mutex_t* mutex) {
	pthread_mutex_unlock(mutex);
}

#endif
