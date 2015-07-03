#include <stdio.h>
#include <stdlib.h>

#include "dx.h"

#include "dx_debug_assert.h"
#include "dx_debug_malloc.h"

#include "dx_util_list.h"

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

int sample_finder(void* data1, void* data2) {
	return data1 - data2;
}

int sample_destroyer(void* data1) {
	FREE(data1);
	return 0;
}
