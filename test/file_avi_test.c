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

#define AVI_LIST_TYPE_RIFF "RIFF"
#define AVI_LIST_TYPE_LIST "LIST"

void file_avi_test(char* path, dx_movie_context_t* movie) {

	dx_movie_context_t* context;
	int i, index;

	context = dx_movie_context_create("assets/drop.avi");
//	context = dx_movie_context_create("/home/in/1.avi");

	ASSERT("AVI파일 Parsing에 실패했습니다.", context != NULL);

	CONSOLE("\nPath : %.128s\n", context->path);

	CONSOLE("\nFrame Rate : %d\n", context->framerate);
	CONSOLE("Total Frames : %d\n", context->total_frame);
	CONSOLE("Play Time : %d\n", context->playtime);
	CONSOLE("Track Count : %d\n", context->track_count);

	CONSOLE("Frame Offset : %ld\n", context->frame_offset);
	CONSOLE("Index Offset : %ld\n", context->index_offset);

	CONSOLE("Width : %d\n", context->width);
	CONSOLE("Height : %d\n", context->height);

	for (i = 0; i < context->track_count; i++) {
		dx_movie_track_info_t* info = context->track_info + i;
		CONSOLE("  Track %.4s : type - %.4s, Handler - %.4s, Rate : %d\n", info->id, info->type, info->handler, info->framerate);
	}

	index = dx_avi_seek_frame(context, context->total_frame - 1, SEEK_SET);

	ASSERT("프레임 갯수 -1 인덱스는 EOF가 아니어야 한다.", dx_movie_frame_eof(context) == 0)
	CONSOLE("Found Index %d\n", index);

	index = dx_avi_seek_frame(context, context->total_frame, SEEK_SET);
	ASSERT("프레임 갯수번째 인덱스는 EOF이다.", dx_movie_frame_eof(context) != 0)

	index = dx_avi_seek_frame(context, 0, SEEK_END);
	ASSERT("현재 프레그먼트의 인덱스는 전체 프레그먼트의 갯수와 같아야 한다.", context->current_frame->fragment_no = context->total_fragment)

	index = dx_avi_seek_frame(context, 0, SEEK_SET);
	ASSERT("현재 프레그먼트의 인덱스는 1 이어야 한다.", context->current_frame->fragment_no = 1)

	index = dx_avi_seek_frame(context, context->total_frame + 1, SEEK_SET);

	ASSERT("마지막 프레임보다 큰 인덱스는 프레임 전체 갯수와 같아야 한다.", index == context->total_frame)
	ASSERT("현재 프레그먼트의 인덱스는 전체 프레그먼트의 갯수와 같아야 한다.", context->current_frame->fragment_no = context->total_fragment)

	dx_movie_context_destroy(context);

	CHKMEM();
}
