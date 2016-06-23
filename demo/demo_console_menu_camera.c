#include "demo.h"

dx_camera_context_t* demo_camera_context = NULL;

void demo_camera_open(char* cmdline) {

	char* path = NULL;

	if(cmdline == NULL || (path = strtok(cmdline, " \t\n\f")) == NULL) {
		ERROR("오픈하고자 하는 Camera Device 이름을 입력해주세요.");
		return;
	}

	if(demo_camera_context != NULL) {
		dx_camera_context_destroy(demo_camera_context);
		demo_camera_context = NULL;
	}

	demo_camera_context = dx_camera_context_create(path);

	if(demo_camera_context == NULL) {
		ERROR("Camera Device 오픈에 실패하였습니다.");
		return;
	}
}

void demo_camera_close(char* cmdline) {
	if(demo_camera_context != NULL) {
		dx_camera_context_destroy(demo_camera_context);
		demo_camera_context = NULL;
	}
}

void demo_camera_info(char* cmdline) {
	if(demo_camera_context == NULL) {
		ERROR("Camera Open Please.");
		return;
	}
	dx_v4l2_print_caps(demo_camera_context->fd);
}
