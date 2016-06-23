// Copyright (c) 2015 - 2015 All Right Reserved, http://hatiolab.com
//
// This source is subject to the Hatio, Lab. Permissive License.
// Please see the License.txt file for more information.
// All other rights reserved.
//
// THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WIHOUT WARRANTY OF ANY
// KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>		// For lseek, read, close
#include <stdint.h>		// For uint32_t, ...
#include <string.h>

#include "dx.h"

#include "dx_debug_assert.h"
#include "dx_debug_malloc.h"

#include "dx_util_log.h"

#include "dx_v4l2.h"
#include "dx_camera.h"

dx_camera_context_t* dx_camera_context_create(char* path) {
	int fd;

	if(dx_v4l2_open_device(path, &fd)) {
		return NULL;
	}

	dx_camera_context_t* pcontext;

	pcontext = (dx_camera_context_t*)MALLOC(sizeof(dx_camera_context_t));
	memset(pcontext, 0x0, sizeof(dx_camera_context_t));

	pcontext->fd = fd;
	pcontext->buffer = NULL;

	return pcontext;
}

void dx_camera_context_destroy(dx_camera_context_t* pcontext) {

	if(pcontext->fd != -1)
		dx_v4l2_close_device(pcontext->fd);

	FREE(pcontext);
}
