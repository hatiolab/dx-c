#include "demo.h"

int demo_video_file = -1;

void demo_video_file_open(char* cmdline) {
	char* path = NULL;

	if(cmdline != NULL && strlen(cmdline) > 0) {
		path = strtok(cmdline, " \t\n\f");
	} else {
		fprintf(stderr, "오픈하고자 하는 파일이름을 입력해주세요.\n");
		return;
	}

	if(demo_video_file >= 0) {
		close(demo_video_file);
		printf("현재 열려있는 비디오 파일을 닫았습니다.\n");
		demo_video_file = -1;
	}

	demo_video_file = dx_h264_open(path);
	if(demo_video_file == -1) {
		perror("비디오파일 열기를 실패하였습니다.");
		fprintf(stderr, "%s\n", path);
		return;
	}

	printf("새로운 비디오 파일을 열었습니다. : %s(%d)\n", path, demo_video_file);
}

void demo_video_file_close(char* cmdline) {
	if(demo_video_file == -1) {
		fprintf(stderr, "현재 열려있는 비디오 파일이 없습니다.\n");
		return;
	}

	dx_h264_close(demo_video_file);

	demo_video_file = -1;
	printf("현재 열려있는 비디오 파일을 닫았습니다.\n");
}

int demo_h264_frame_info_callback(int fd, dx_buffer_t* buffer, int idx) {
	printf("프레임 %d (%d)\n", idx, dx_buffer_remains(buffer));

	return 0; /* keep going to the end frame */
}

void demo_video_file_info(char* cmdline) {
	if(demo_video_file < 0) {
		fprintf(stderr, "먼저 비디오 파일을 열어주세요.\n");
		return;
	}

	dx_h264_frame_iterate(demo_video_file, demo_h264_frame_info_callback);
}
