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
#include <arpa/inet.h>

#include "dx.h"

#include "dx_debug_assert.h"
#include "dx_debug_malloc.h"

#include "dx_util_log.h"

#include "dx_file_movie.h"
#include "dx_file_avi.h"

#include "dx_net_packet_file.h"

dx_movie_context_t* dx_movie_context_create(char* path) {

	int fd = open(path, O_RDONLY);

	if(fd < 0)
		return NULL;

	/*
	 * TODO 동영상 타입별로 동적으로 파서를 구분해서 호출해야 한다. 현재는 AVI타입 뿐이므로...
	 */
	dx_movie_context_t* context = dx_avi_parse_scheme(fd);

	if(context != NULL)
		strncpy(context->path, path, DX_PATH_MAX_SIZE);

	return context;
}

void dx_movie_context_destroy(dx_movie_context_t* context) {

	close(context->fd);

	if(context->current_frame != NULL) {
		FREE(context->current_frame);
		context->current_frame = NULL;
	}
	FREE(context);
}

/*
 * 현재 인덱스를 옮김.
 * 현재 프레임 넘버를 반환함.
 */
int dx_movie_seek_frame(dx_movie_context_t* context, int offset, int whence) {
	return dx_avi_seek_frame(context, offset, whence);
}

/*
 * 현재 인덱스에서 프레임을 가져오고, 인덱스를 하나 증가시킴.
 * 현재 프레임 인덱스 정보를 담은 주소를 반환함.
 */
dx_movie_frame_index_t* dx_movie_get_frame(dx_movie_context_t* context) {
	return dx_avi_get_frame_index(context);
}

/*
 * 현재 프레임의 인덱스가 마지막 프레임인가를 테스트함.
 */
int dx_movie_frame_eof(dx_movie_context_t* context) {
	return context->total_frame > context->current_frame->frame_no ? 0 : 1;
}
