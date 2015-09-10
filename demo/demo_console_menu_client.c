#include "demo.h"

void demo_client_send_handler(char* cmdline){}

void demo_client_hb_handler(char* cmdline) {
	if(demo_client < 0){
		ERROR("Demo Client is not started..");
		return;
	}
	dx_packet_send_heartbeat(demo_client, 0);
}

dx_schedule_t* demo_client_repeat_hb_scheduler = NULL;

int demo_client_hb_callback(void* p) {
	if(demo_client < 0){
		ERROR("Cannot Send HeartBeat Repeatedly - Demo Client is not started.");
		ERROR("Schedule will be canceled.");
		dx_schedule_cancel(demo_client_repeat_hb_scheduler);
		demo_client_repeat_hb_scheduler = NULL;
		return -1;
	}
	return dx_packet_send_heartbeat(demo_client, 0);
}

void demo_client_repeat_hb_handler(char* cmdline) {
	char* param;
	int duration = 5000; /* default heartbeat duration */

	if(demo_client < 0){
		ERROR("Cannot Register Repeatable HeartBeat Schedule - Demo Client is not started.");
		return;
	}

	if(cmdline != NULL && (param = strtok(cmdline, " \t\n\f")) != NULL) {
		duration = atoi(param);
	}

	if(duration < 1000)
		duration = 5000;

	demo_client_repeat_hb_scheduler = dx_schedule_register(duration, duration, 1/* repeatable */, demo_client_hb_callback, NULL);
}

void demo_client_repeat_hb_stop_handler(char* cmdline) {
	if(demo_client_repeat_hb_scheduler != NULL)
		dx_schedule_cancel(demo_client_repeat_hb_scheduler);
}

void demo_client_event_send_handler(char* cmdline){
	dx_packet_send_event_u32(demo_client, 100, 1000);
}

void demo_client_file_handler(char* cmdline){
}

void demo_client_stop_handler(char* cmdline) {
	close(demo_client);
	demo_client = -1;
}
