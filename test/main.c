#include <stdio.h>
#include <stdlib.h>

#include "dx_util_buffer.h"

void util_buffer_test();
void debug_malloc_test();
void util_lock_test();

int main(void) {

	printf("\nTest - util_buffer_test()..\n");
	util_buffer_test();
	printf("..Done\n");

	printf("\nTest - debug_malloc_test()..\n");
	debug_malloc_test();
	printf("..Done\n");

	printf("\nTest - util_lock_test()..\n");
	util_lock_test();
	printf("..Done\n");

	return EXIT_SUCCESS;
}
