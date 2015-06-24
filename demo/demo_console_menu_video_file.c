#include "demo.h"

#define FRAME_BOUNDARY 0x01000000

int demo_video_file = -1;
int demo_video_file_current_frame = 0;

dx_buffer_t* frame_buffer = NULL;

dx_buffer_t* demo_read_a_frame(int count) {

	uint32_t buffer;
	int nread;
	int tread = 0; /* read total */

	/* 첫 Boundary를 찾는다. */
	if(count == 0) {

		while((nread = read(demo_video_file, &buffer, 4)) == 4) {
			if(FRAME_BOUNDARY == buffer) {
				dx_buffer_clear(frame_buffer);
				break;
			}
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
	dx_buffer_clear(frame_buffer);

	while((nread = read(demo_video_file, &buffer, 4)) == 4) {
		if(FRAME_BOUNDARY == buffer) {
			dx_buffer_flip(frame_buffer);
			break;
		}
		dx_buffer_put(frame_buffer, &buffer, 4);
		tread += nread;
	}

	if(nread != 4) {
		printf("Seek Boundary for %d bytes.\n", tread + nread);
		return NULL;
	}

	return frame_buffer;
}

void demo_video_file_frame() {
	dx_buffer_t* frame;
	int count = 0;

	if(demo_video_file < 0) {
		fprintf(stderr, "Read video file first..\n");
		return;
	}

	if(frame_buffer == NULL)
		frame_buffer = dx_buffer_alloc(1024 * 1024);

	while((frame = demo_read_a_frame(count)) != NULL) {
		count++;
		printf("Frame %d (%d)\n", count, dx_buffer_remains(frame));
	}
}

void demo_video_file_read(char* cmdline) {
	char* path = NULL;

	if(demo_video_file >= 0) {
		close(demo_video_file);
		demo_video_file = -1;
	}

	if(cmdline != NULL && strlen(cmdline) > 0) {
		path = strtok(cmdline, " \t\n\f");
	} else {
		fprintf(stderr, "Filename required.\n");
		return;
	}

	demo_video_file = open(path, O_RDONLY);

	if(demo_video_file == -1) {
		perror("Read video file failed.");
		fprintf(stderr, "%s\n", path);
		return;
	}

	printf("Read video file Successfully : %s(%d)\n", path, demo_video_file);
	demo_video_file_frame();
}
