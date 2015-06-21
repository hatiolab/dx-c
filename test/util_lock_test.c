#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "dx_util_lock.h"
#include "dx_debug_assert.h"

int _util_lock_test_counter;

void* util_lock_test_task(void* arg) {
	dx_lock();

	unsigned long i = 0;

	for(i = 0;i < 0xFFFFFFL;i++)
		_util_lock_test_counter++;

	dx_unlock();

	return NULL;
}

void util_lock_test() {
	int i;

	dx_lock_init();

	pthread_t tid[16];

	for(i = 0;i < 16;i++)
		ASSERT("Can't create Thread\n", pthread_create(&tid[i], NULL, &util_lock_test_task, NULL) == 0)

	for(i = 0;i < 16;i++)
		pthread_join(tid[i], NULL);

	ASSERT("Lock dosen't work.", _util_lock_test_counter == 0xFFFFFFL * 16)

	dx_lock_destroy();
}
