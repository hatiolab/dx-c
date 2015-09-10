#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "dx.h"

#include "dx_debug_assert.h"
#include "dx_debug_malloc.h"

#include "dx_util_lock.h"
#include "dx_util_log.h"

#define UTIL_LOCK_TEST_THREAD_COUNT 16

int _util_lock_test_counter;

void* util_lock_test_task(void* arg) {

	unsigned long i = 0;

	DX_LOCK_GLOBAL()
	DX_LOCK_GLOBAL()

	for(i = 0;i < 0xFFFFFFL;i++) {
		_util_lock_test_counter++;
	}

	DX_UNLOCK_GLOBAL()
	DX_UNLOCK_GLOBAL()

	return NULL;
}

#ifdef DX_MULTITHREADED

void util_lock_test() {
	int i;

	_util_lock_test_counter = 0;

	DX_LOCK_GLOBAL_INIT()

	pthread_t tid[UTIL_LOCK_TEST_THREAD_COUNT];

	for(i = 0;i < UTIL_LOCK_TEST_THREAD_COUNT;i++)
		ASSERT("Can't create Thread\n", pthread_create(&tid[i], NULL, &util_lock_test_task, NULL) == 0)

	for(i = 0;i < UTIL_LOCK_TEST_THREAD_COUNT;i++)
		pthread_join(tid[i], NULL);

	ASSERT("Lock dosen't work.", _util_lock_test_counter == 0xFFFFFFL * UTIL_LOCK_TEST_THREAD_COUNT)

	CHKMEM()

	DX_LOCK_GLOBAL_DESTROY()
}

#else

void util_lock_test() {
	CONSOLE("This test only for multithreaded environment.\n")
}

#endif
