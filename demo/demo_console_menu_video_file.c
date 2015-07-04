#include "demo.h"

dx_movie_context_t* demo_movie_file_context = NULL;

void demo_video_file_open(char* cmdline) {

	char* path = NULL;

	if(cmdline == NULL || (path = strtok(cmdline, " \t\n\f")) == NULL) {
		ERROR("오픈하고자 하는 파일이름을 입력해주세요.");
		return;
	}

	if(demo_movie_file_context != NULL) {
		close(demo_movie_file_context->fd);
		CONSOLE("현재 열려있는 비디오 파일을 닫았습니다.\n");
		dx_movie_context_destroy(demo_movie_file_context);
		demo_movie_file_context = NULL;
	}

	demo_movie_file_context = dx_movie_context_create(path);

	if(demo_movie_file_context == NULL) {
		ERROR("동영상 파일 오픈에 실패하였습니다.");
		return;
	}

	CONSOLE("\nFrame Rate : %d\n", demo_movie_file_context->framerate);
	CONSOLE("Total Frames : %d\n", demo_movie_file_context->total_frame);
	CONSOLE("Total Fragments : %d\n", demo_movie_file_context->total_fragment);
	CONSOLE("Play Time : %d\n", demo_movie_file_context->playtime);
	CONSOLE("Track Count : %d\n", demo_movie_file_context->track_count);

	CONSOLE("Frame Offset : %ld\n", demo_movie_file_context->frame_offset);
	CONSOLE("Index Offset : %ld\n", demo_movie_file_context->index_offset);

	CONSOLE("Width : %d\n", demo_movie_file_context->width);
	CONSOLE("Height : %d\n", demo_movie_file_context->height);

	dx_movie_seek_frame(demo_movie_file_context, 0, SEEK_SET); // TODO goto startframe

	CONSOLE("새로운 비디오 파일을 열었습니다. : %s\n", path);
}

void demo_video_file_close(char* cmdline) {
	if(demo_movie_file_context == NULL) {
		ERROR("현재 열려있는 비디오 파일이 없습니다.");
		return;
	}

	dx_movie_context_destroy(demo_movie_file_context);
	demo_movie_file_context = NULL;

	CONSOLE("현재 열려있는 비디오 파일을 닫았습니다.\n");
}

int demo_h264_frame_info_callback(int fd, dx_buffer_t* buffer, int idx) {
	CONSOLE("프레임 %d (%d)\n", idx, dx_buffer_remains(buffer));

	return 0; /* keep going to the end frame */
}

void demo_video_file_info(char* cmdline) {
	if(demo_movie_file_context == NULL) {
		ERROR("먼저 비디오 파일을 열어주세요.");
		return;
	}
}
