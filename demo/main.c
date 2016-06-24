#include "demo.h"

int demo_server = -1;
int demo_client = -1;
int demo_discovery_server = -1;
int demo_discovery_client = -1;

int quit = 0;

dx_console_menu_t demo_console_menus[]  = {
	{1, "start", "", "start servers ..", NULL},
		{10, "ds", "[port]", "start discovery-listener", demo_start_discovery_server_handler},
		{11, "dc", "[port]", "start discovery-requester", demo_start_discovery_client_handler},
		{12, "server", "[port]", "start server", demo_start_server_handler},
		{13, "client", "[hostname] [port]", "start client", demo_start_client_handler},
		{14, "discovery", "[port]", "start client through discovery request", demo_discovery_send_handler},
		{15, "all", "[port]", "start all services on the localhost", demo_start_all_handler},
		{16, "sall", "[port]", "start all services for server", demo_start_all_server_handler},
		{17, "call", "[port]", "start all services for client", demo_start_all_client_handler},
	{2, "server", "", "do something with server ..",  NULL},
		{20, "send", "", "send some packet to clients",  demo_server_send_handler},
		{21, "hb", "", "send heartbeat to clients",  demo_server_hb_handler},
		{22, "event", "", "send some events to clients",  demo_server_event_send_handler},
		{23, "file", "", "send a file to clients", demo_server_file_handler},
		{24, "stop", "", "stop server", demo_server_stop_handler},
	{3, "client", "", "do something with client ..",  NULL},
		{30, "send", "", "send some packet to server",  demo_client_send_handler},
		{31, "hb", "", "send heartbeat to server",  demo_client_hb_handler},
		{32, "repeat-hb", "[duration]", "send heartbeat to server repeatedly",  demo_client_repeat_hb_handler},
		{33, "event", "", "send some events to server",  demo_client_event_send_handler},
		{34, "file", "", "send a file to server",  demo_client_file_handler},
		{35, "stop", "", "stop client",  demo_client_stop_handler},
	{4, "movie", "", "do something about movie playback ..", NULL},
		{40, "movie-info", "[filepath]", "send request movie file information to server",  demo_movie_get_movie_info_handler},
		{41, "start-movie", "[filepath]", "send request start movie play to server",  demo_movie_start_handler},
		{42, "stop-movie", "[filepath]", "send request stop movie play to server",  demo_movie_stop_handler},
		{43, "pbstart", "[filepath]", "send a playback start command to server",  demo_movie_playback_start_handler},
		{44, "pbstop", "", "send a playback stop command to server",  demo_movie_playback_stop_handler},
	{5, "video", "", "do something about video/camera ..", NULL},
		{50, "file", "", "do something about video file ..", NULL},
			{500, "open", "[filename]", "open video file", demo_video_file_open},
			{501, "info", "", "read video frames, print frame info", demo_video_file_info},
			{502, "close", "", "close video file", demo_video_file_close},
	{6, "camera", "", "do something about camera ..", NULL},
		{60, "open", "[device]", "open camera", demo_camera_open},
		{61, "info", "", "print camera info", demo_camera_info},
		{62, "capture", "", "capture image", demo_camera_capture},
		{63, "close", "", "close camera", demo_camera_close},
	{0},
};

void demo_exit_handler() {
	quit = 1;
}

int main() {

	dx_event_mplexer_create();
	dx_scheduler_start();

	dx_console_start(demo_console_menus, demo_exit_handler);

	/* Big Loop */
	while(!quit) {
		dx_event_mplexer_poll(dx_scheduler_next_wait());
		dx_scheduler_do();
	}

	dx_scheduler_stop();
	dx_event_mplexer_destroy();

	CHKMEM();

	return 0;
}
