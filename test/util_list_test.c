#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "dx.h"

#include "dx_debug_assert.h"
#include "dx_debug_malloc.h"

#include "dx_util_list.h"
#include "dx_util_log.h"

int sample_finder(void* data1, void* data2);
int sample_destroyer(void* data1);

void util_list_test() {
	dx_list_t list;
	int i = 0;

	dx_list_init(&list, sample_finder, sample_destroyer);

	for(i = 0;i < 100;i++) {
		dx_list_add(&list, MALLOC(12));
	}

	ASSERT("List Size should be 100", dx_list_size(&list) == 100)

	dx_list_clear(&list);

	ASSERT("List Size should be 0", dx_list_size(&list) == 0)

	CHKMEM();
}

dx_list_t list_for_thread_test;

void* thread_list_tester(void* arg) {
	int i = 0, j = 0;
	void* data[1000];

	for(j = 0;j < 1000;j++) {
		for(i = 0;i < 100;i++) {
			data[i] = MALLOC(24);
			dx_list_add(&list_for_thread_test, data[i]);
		}

		for(i = 0;i < 100;i++) {
			dx_list_remove(&list_for_thread_test, data[i]);
		}
	}

	CONSOLE("ENDED\n");
	return NULL;
}

void util_list_thread_test() {
	pthread_t pid[2];

	dx_list_init(&list_for_thread_test, sample_finder, sample_destroyer);

	ASSERT("Can't create Thread\n", pthread_create(&pid[0], NULL, &thread_list_tester, NULL) == 0)
	ASSERT("Can't create Thread\n", pthread_create(&pid[1], NULL, &thread_list_tester, NULL) == 0)

	pthread_join(pid[0], NULL);
	pthread_join(pid[1], NULL);

	ASSERT("List Size should be 0", dx_list_size(&list_for_thread_test) == 0)

	dx_list_clear(&list_for_thread_test);

	ASSERT("List Size should be 0", dx_list_size(&list_for_thread_test) == 0)

	dx_list_close(&list_for_thread_test);

	CHKMEM();
}

int sample_finder(void* data1, void* data2) {
	return data1 - data2;
}

int sample_destroyer(void* data1) {
	FREE(data1);
	return 0;
}
