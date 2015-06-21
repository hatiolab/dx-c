#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "dx_util_lock.h"
#include "dx_debug_assert.h"

#define UTIL_LOCK_TEST_THREAD_COUNT 16

int _util_lock_test_counter;

void* util_lock_test_task(void* arg) {

	unsigned long i = 0;

	dx_lock();

	for(i = 0;i < 0xFFFFFFL;i++) {
		_util_lock_test_counter++;
	}

	dx_unlock();

	return NULL;
}

void util_lock_test() {
	int i;

	_util_lock_test_counter = 0;

	dx_lock_init();

	pthread_t tid[UTIL_LOCK_TEST_THREAD_COUNT];

	for(i = 0;i < UTIL_LOCK_TEST_THREAD_COUNT;i++)
		ASSERT("Can't create Thread\n", pthread_create(&tid[i], NULL, &util_lock_test_task, NULL) == 0)

	for(i = 0;i < UTIL_LOCK_TEST_THREAD_COUNT;i++)
		pthread_join(tid[i], NULL);

	ASSERT("Lock dosen't work.", _util_lock_test_counter == 0xFFFFFFL * UTIL_LOCK_TEST_THREAD_COUNT)

	printf("..Done\n");

	dx_lock_destroy();
}
