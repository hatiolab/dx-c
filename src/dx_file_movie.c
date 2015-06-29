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

#include "dx_debug_assert.h"
#include "dx_debug_malloc.h"

#include "dx_util_log.h"

#include "dx_file_movie.h"
#include "dx_file_avi.h"

dx_movie_context_t* dx_movie_context_crate(char* path) {

	int fd = open(path, O_RDONLY);

	/*
	 * TODO 동영상 타입별로 동적으로 파서를 구분해서 호출해야 한다. 현재는 AVI타입 뿐이므로...
	 */
	return dx_avi_parse_scheme(fd);
}

void dx_movie_context_destroy(dx_movie_context_t* context) {

	close(context->fd);

	if(context->current_index != NULL) {
		FREE(context->current_index);
		context->current_index = NULL;
	}
	FREE(context);
}

dx_movie_frame_index_t* dx_movie_get_index_for_frame_no(dx_movie_context_t* context, int offset, int whence) {
	uint16_t frame_no = offset;
	int i = 0;

	if(context->current_index == NULL) {
		/* 여기서 초기화 작업을 한다. */
	}

//	context->current_index->frame_no = dx_avi_seek_by_frame_offset(context, offset, whence, context->current_index);

	/*
	 * 아래부분은 dx_avi_seek_by_frame_offset으로 이동.
	 */
//	switch(whence) {
//	case SEEK_SET:
//		/*
//		 * int dx_avi_find_index_by_frame_no(dx_movie_context_t* context, int nframe);
//		 * 를 사용해서 인덱스 NO를 찾는다.
//		 * 위 함수도 whence를 사용하도록 수정한다.
//		 */
//		frame_no = offset;
//		break;
//	case SEEK_CUR:
//		frame_no = context->current_index->frame_no + offset;
//		break;
//	case SEEK_END:
//		frame_no = context->total_frame + offset;
//		break;
//	}

//	for(i = 0;i < context->current_index;i++) {
//		dx_movie_frame_track_index_t index = context->current_index->track[i];
//
//		index.length = 10;
//		index.offset = 100;
//		index.track_id
//	}



	return context->current_index;
}
