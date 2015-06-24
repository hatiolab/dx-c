#include "demo.h"

int demo_video_file = -1;
int demo_video_file_current_frame = 0;

void demo_video_file_read(char* cmdline) {
	char* path;

	if(demo_video_file >= 0) {
		close(demo_video_file);
		demo_video_file = -1;
	}

	if(cmdline != NULL && strlen(cmdline) > 0)
		path = strtok(cmdline, " \t\n\f");

	demo_video_file = open(path, O_RDONLY);

	if(demo_video_file == -1) {
		perror("Read video file failed.");
		printf("%s\n", path);
		return;
	}

	printf("Read video file Successfully : %s(%d)\n", path, demo_video_file);
}

void demo_video_file_frame(char* cmdline) {
	if(demo_video_file >= 0) {
		printf("Read video file first..\n");
		return;
	}
}
