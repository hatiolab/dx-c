#include <stdio.h>
#include <stdlib.h>

#include "dx.h"

#include "dx_debug_assert.h"
#include "dx_debug_malloc.h"

#include "dx_util_buffer.h"

void util_buffer_alloc_test();
void util_buffer_wrap_test();

void util_buffer_test() {
	util_buffer_alloc_test();
	util_buffer_wrap_test();
}

void util_buffer_alloc_test() {
	dx_buffer_t* pbuf = dx_buffer_alloc(1000);
	uint8_t psrc[100] = {
		[99] = (uint8_t)0x8,
	};
	uint8_t pdes[100];
	int ret;

	ASSERT("Buffer Capacity should be 1000", dx_buffer_capacity(pbuf) == 1000)
	ASSERT("Buffer Position should be 0", dx_buffer_getpos(pbuf) == 0)
	ASSERT("Buffer Remains should be 1000", dx_buffer_remains(pbuf) == 1000)

	dx_buffer_put(pbuf, psrc, 100);
	ASSERT("Buffer Position should be 100", dx_buffer_getpos(pbuf) == 100)

	dx_buffer_flip(pbuf);
	ASSERT("Buffer Position should be 0", dx_buffer_getpos(pbuf) == 0)
	ASSERT("Buffer Limit should be 100", dx_buffer_getlimit(pbuf) == 100)
	ASSERT("Buffer Remains should be 100", dx_buffer_remains(pbuf) == 100)

	ret = dx_buffer_get(pbuf, pdes, 100);
	ASSERT("Buffer Get should return 100", ret == 100)
	ASSERT("Buffer Position should be 100", dx_buffer_getpos(pbuf) == 100)
	ASSERT("Destination[99] should be 0x8", pdes[99] == 0x8)
	ASSERT("Buffer Remains should be 0", dx_buffer_remains(pbuf) == 0)

	dx_buffer_free(pbuf);

	CHKMEM();
}

void util_buffer_wrap_test() {
	int8_t* pdata = (int8_t*)MALLOC(1100);
	dx_buffer_t* pbuf = dx_buffer_wrap(pdata, 1000);
	uint8_t psrc[100] = {
		[99] = (uint8_t)0x8,
	};
	uint8_t pdes[100];
	int ret;

	ASSERT("Buffer Capacity should be 1000", dx_buffer_capacity(pbuf) == 1000)
	ASSERT("Buffer Position should be 0", dx_buffer_getpos(pbuf) == 0)
	ASSERT("Buffer Remains should be 1000", dx_buffer_remains(pbuf) == 1000)

	dx_buffer_put(pbuf, psrc, 100);
	ASSERT("Buffer Position should be 100", dx_buffer_getpos(pbuf) == 100)

	dx_buffer_flip(pbuf);
	ASSERT("Buffer Position should be 0", dx_buffer_getpos(pbuf) == 0)
	ASSERT("Buffer Limit should be 100", dx_buffer_getlimit(pbuf) == 100)
	ASSERT("Buffer Remains should be 100", dx_buffer_remains(pbuf) == 100)

	ret = dx_buffer_get(pbuf, pdes, 100);
	ASSERT("Buffer Get should return 100", ret == 100)
	ASSERT("Buffer Position should be 100", dx_buffer_getpos(pbuf) == 100)
	ASSERT("Destination[99] should be 0x8", pdes[99] == 0x8)
	ASSERT("Buffer Remains should be 0", dx_buffer_remains(pbuf) == 0)

	dx_buffer_free(pbuf);

	CHKMEM();
}
