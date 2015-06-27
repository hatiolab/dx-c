#include "demo.h"

void demo_client_send_handler(char* cmdline){}

void demo_client_hb_handler(char* cmdline) {
	if(demo_client < 0){
		fprintf(stderr, "Demo Client is not started..\n");
		return;
	}
	dx_packet_send_heartbeat(demo_client, 0);
}

dx_schedule_t* demo_client_repeat_hb_scheduler = NULL;

void demo_client_hb_callback(void* p) {
	if(demo_client < 0){
		fprintf(stderr, "Cannot Send HeartBeat Repeatedly - Demo Client is not started.\n");
		fprintf(stderr, "Schedule will be canceled.\n");
		dx_schedule_cancel(demo_client_repeat_hb_scheduler);
		demo_client_repeat_hb_scheduler = NULL;
		return;
	}
	dx_packet_send_heartbeat(demo_client, 0);
}

void demo_client_repeat_hb_handler(char* cmdline) {
	int duration = 5000; /* default heartbeat duration */

	if(demo_client < 0){
		fprintf(stderr, "Cannot Register Repeatable HeartBeat Schedule - Demo Client is not started.\n");
		return;
	}

	if(cmdline != NULL && strlen(cmdline) > 0)
		duration = atoi(strtok(cmdline, " \t\n\f"));

	if(duration < 1000)
		duration = 5000;

	demo_client_repeat_hb_scheduler = dx_schedule_register(duration, duration, 1/* repeatable */, demo_client_hb_callback, NULL);
}

void demo_client_repeat_hb_stop_handler(char* cmdline) {
	if(demo_client_repeat_hb_scheduler != NULL)
		dx_schedule_cancel(demo_client_repeat_hb_scheduler);
}

void demo_client_event_send_handler(char* cmdline){}
void demo_client_file_handler(char* cmdline){}

void demo_client_playback_start_handler(char* cmdline) {
	char* path = NULL;

	if(demo_client < 0){
		fprintf(stderr, "Demo Client is not started..\n");
		return;
	}

	if(cmdline != NULL && strlen(cmdline) > 0)
		path = strtok(cmdline, " \t\n\f");

	dx_packet_send_string(demo_client, DX_PACKET_TYPE_COMMAND, OD_CMD_START_PLAYBACK, path);
}

void demo_client_playback_stop_handler(char* cmdline) {
	if(demo_client < 0){
		fprintf(stderr, "Demo Client is not started..\n");
		return;
	}
	dx_packet_send_command_u32(demo_client, OD_CMD_STOP_PLAYBACK, 0);
}

void demo_client_stop_handler(char* cmdline) {
	close(demo_client);
}

