/*
 ============================================================================
 Name        : dx-c-sample.c
 Author      : hatiolab
 Version     :
 Copyright   : Just Free
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>

#include "dx_util_buffer.h"

void util_buffer_test() {
	dx_buffer_t* pbuf = dx_buffer_alloc(100);

	dx_buffer_free(pbuf);
}
