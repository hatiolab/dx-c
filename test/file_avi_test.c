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

#define AVI_LIST_TYPE_RIFF "RIFF"
#define AVI_LIST_TYPE_LIST "LIST"

void file_avi_test(char* path, dx_movie_context_t* movie) {

	dx_movie_context_t* context;
	int i, index;

	context = dx_movie_context_crate("assets/drop.avi");
//	context = dx_movie_context_crate("/home/in/1.avi");

	ASSERT("AVI파일 Parsing에 실패했습니다.", context != NULL);

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

	index = dx_avi_find_index_by_frame_no(context, context->total_frame);

	ASSERT("마지막 프레임의 인덱스는 찾을 수 있어야 한다.", index != -1)
	CONSOLE("Found Index %d\n", index);

	index = dx_avi_find_index_by_frame_no(context, context->total_frame + 1);

	ASSERT("마지막 프레임보다 큰 인덱스는 찾을 수 없어야 한다.", index == -1)

	dx_movie_context_destroy(context);

	CHKMEM();
}
