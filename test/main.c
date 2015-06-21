#include <stdio.h>
#include <stdlib.h>

#include "dx_util_buffer.h"

void util_buffer_test();
void debug_malloc_test();

int main(void) {
	puts("Hello Things!"); /* prints Hello Things! */

	util_buffer_test();
//	debug_malloc_test();

	return EXIT_SUCCESS;
}
