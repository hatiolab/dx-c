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

/*
 * 프레임번호로 프레임을 찾는 방법은 두가지가 있다.
 * 1. 인덱스(idx1)청크에서 찾는 방법.
 * 2. 무비 리스트(movi)에서 직접 찾는 방법.
 *
 * 여기에서는 인덱스 청크에서 찾는 방법을 제공한다.
 * 찾는 방법 -
 * 찾고자 하는 프레임 번호를 n이라고 하면,
 * 0번째 인덱스 청크부터 순차적으로 뒤져가면서, 트랙이름이 "00"으로 시작하는 n번째의 청크의 인덱스를 반환한다.
 */

/*
 * 현재 인덱스를 옮김.
 */
int dx_avi_seek_frame(dx_movie_context_t* context, int offset, int whence) {
	int frame_no;
	int i = 0;
	int found_frame = 0;
	char track_name[4];

	ASSERT("Movie Context 초기화가 되어있지 않습니다.", context->current_frame != NULL)

	switch(whence) {
	case SEEK_SET:
		/*
		 * int dx_avi_find_index_by_frame_no(dx_movie_context_t* context, int nframe);
		 * 를 사용해서 인덱스 NO를 찾는다.
		 * 위 함수도 whence를 사용하도록 수정한다.
		 */
		frame_no = offset;
		break;
	case SEEK_CUR:
		frame_no = context->current_frame->frame_no + offset;
		break;
	case SEEK_END:
		frame_no = context->total_frame + offset;
		break;
	}

	if(frame_no >= context->total_frame) {
		context->current_frame->fragment_no = context->total_fragment;
		context->current_frame->frame_no = context->total_frame;

		return context->current_frame->frame_no;
	} else {

		for(i = 0;found_frame < context->total_frame;i++) {
			lseek(context->fd, context->index_offset + (i * sizeof(dx_avi_index_entry_t)), SEEK_SET);
			read(context->fd, track_name, 4);
			if(memcmp(track_name, "00", 2) == 0 && frame_no == found_frame++) {
				context->current_frame->frame_no = frame_no;
				context->current_frame->fragment_no = i;

				CONSOLE("++++ SEEK Current Frame : %d, Current Fragment : %d\n", context->current_frame->frame_no, context->current_frame->fragment_no);

				return i;
			}
		}
	}

	return context->current_frame->frame_no;
}

/*
 * 현재 인덱스에서 각 트랙의 인덱스 정보를 가져옴.
 */
dx_movie_frame_index_t* dx_avi_get_frame_index(dx_movie_context_t* context) {

	int i = 0;
	int frame_length = 0;
	int track_count = 0;
	int current_fragment_no = context->current_frame->fragment_no;
	dx_avi_index_entry_t entry;

	/*
	 * 현재 프레임의 fragment_no부터 다음 프레임까지 읽어서, 현재 프레임의 인덱스 리스트에 값을 채운다.
	 */

	memset(context->current_frame->track, 0x0, sizeof(dx_movie_frame_track_index_t)*context->track_count);

	lseek(context->fd, context->index_offset + (current_fragment_no * sizeof(dx_avi_index_entry_t)), SEEK_SET);

	for(i = 0;i < context->track_count && current_fragment_no + i < context->total_fragment;i++) {

		dx_movie_frame_track_index_t* index = context->current_frame->track + i;

		read(context->fd, &entry, sizeof(entry));

		if(i != 0 && memcmp(entry.ckid, "00", 2) == 0)
			break;

		memcpy(index->track_id, entry.ckid, 4);

		index->length = entry.length;
		index->offset = entry.offset;
		index->flag = entry.flags;
		frame_length += index->length;
		track_count++;

	}

	if(i > 0) {
		context->current_frame->track_count = track_count;
		context->current_frame->frame_length = frame_length;
	}

	return context->current_frame;
}

/*
 * 현재 인덱스에서 프레임데이타를 가져옴. (프레임데이타 : 각 트랙의 모든 프레그먼트 데이타를 포함한 벌크를 의미함)
 */
int dx_avi_get_frame_data(dx_movie_context_t* context, int8_t* buffer) {
	int i = 0;
	int offset = 0;
	dx_movie_frame_index_t* current_frame = context->current_frame;

	for(i = 0;i < current_frame->track_count;i++) {
		dx_movie_frame_track_index_t* index = current_frame->track + i;


		lseek(context->fd, context->frame_offset + index->offset, SEEK_SET);
		read(context->fd, buffer + offset, index->length);

		offset += index->length;
	}

	/*
	 * 현재 프레임 위치를 다음 프레임으로 이동함.
	 */
	if(context->current_frame->frame_no < context->total_frame)
		context->current_frame->frame_no++;

	if(context->current_frame->fragment_no < context->total_fragment)
		context->current_frame->fragment_no += current_frame->track_count;
	else
		context->current_frame->fragment_no = context->total_fragment;

	return offset;
}
