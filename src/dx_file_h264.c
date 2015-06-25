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
#include <fcntl.h>
#include <unistd.h>	// For lseek, read, close

#include "dx_util_buffer.h"

#include "dx_file_h264.h"

dx_buffer_t* dx_frame_buffer = NULL;

dx_buffer_t* dx_h264_read_frame(int fd, dx_buffer_t* bytebuffer, int frame_number) {

	uint32_t buf4bytes;
	int nread;
	int tread = 0; /* read total */

	dx_buffer_clear(bytebuffer);

	/* 첫 Boundary를 찾는다. */
	if(frame_number == 0) {
		lseek(fd, SEEK_SET, 0);

		while((nread = read(fd, &buf4bytes, 4)) == 4) {
			if(DX_H264_FRAME_BOUNDARY == buf4bytes)
				break;

			tread += nread;
		}

		tread += nread;
		printf("Seek Boundary for %d bytes.\n", tread);

		if(nread != 4) {
			return NULL;
		}
	}

	/* 다음 Boundary를 찾는다. */
	tread = 0;

	while((nread = read(fd, &buf4bytes, 4)) == 4) {
		if(DX_H264_FRAME_BOUNDARY == buf4bytes)
			break;

		dx_buffer_put(bytebuffer, &buf4bytes, 4);
		tread += nread;
	}

	if(nread != 4) {
		printf("Seek Boundary for %d bytes.\n", tread + nread);
		return NULL;
	}

	dx_buffer_flip(bytebuffer);

	return bytebuffer;
}

void dx_h264_frame_iterate(int fd, dx_h264_frame_callback callback) {
	dx_buffer_t* frame;
	int idx = 0;

	if(dx_frame_buffer == NULL)
		dx_frame_buffer = dx_buffer_alloc(DX_H264_FRAME_MAX_SIZE);

	while((frame = dx_h264_read_frame(fd, dx_frame_buffer, idx)) != NULL) {
		if(callback(fd, frame, idx) != 0)
			break;
		idx++;
	}
}

void dx_h264_close(int fd) {
	close(fd);
}

int dx_h264_open(char* path) {
	return open(path, O_RDONLY);
}
