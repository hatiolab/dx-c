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

#include "dx.h"

#include "dx_debug_malloc.h"

void debug_malloc_test() {
	void* p = NULL;

	p = MALLOC(32);

	printf("First Try for Free\n");
	FREE(p);

//	CONSOLE("Second Try for Free\n");
//	FREE(p);

	CHKMEM();
}
