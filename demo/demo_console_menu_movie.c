#include "demo.h"

void demo_movie_playback_start_handler(char* cmdline) {
	char* path = NULL;

	if(demo_client < 0){
		ERROR("Demo Client is not started..");
		return;
	}

	if(cmdline != NULL && strlen(cmdline) > 0)
		path = strtok(cmdline, " \t\n\f");

	dx_packet_send_string(demo_client, DX_PACKET_TYPE_COMMAND, OD_CMD_START_PLAYBACK, path);
}

void demo_movie_playback_stop_handler(char* cmdline) {
	if(demo_client < 0){
		ERROR("Demo Client is not started..");
		return;
	}
	dx_packet_send_command_u32(demo_client, OD_CMD_STOP_PLAYBACK, 0);
}

void demo_movie_get_movie_info_handler(char* cmdline) {
	char* path = NULL;

	if(demo_client < 0){
		ERROR("Demo Client is not started..");
		return;
	}

	if(cmdline != NULL && strlen(cmdline) > 0)
		path = strtok(cmdline, " \t\n\f");

	dx_packet_send_movie_get_info(demo_client, path);
}

void demo_movie_start_handler(char* cmdline) {
	char* path = NULL;

	if(demo_client < 0){
		ERROR("Demo Client is not started..");
		return;
	}

	if(cmdline != NULL && strlen(cmdline) > 0)
		path = strtok(cmdline, " \t\n\f");

	dx_packet_send_movie_start(demo_client, path, 0, -1, 30);
}

void demo_movie_stop_handler(char* cmdline) {
	char* path = NULL;

	if(demo_client < 0){
		ERROR("Demo Client is not started..");
		return;
	}

	if(cmdline != NULL && strlen(cmdline) > 0)
		path = strtok(cmdline, " \t\n\f");

	dx_packet_send_movie_stop(demo_client, path);
}
