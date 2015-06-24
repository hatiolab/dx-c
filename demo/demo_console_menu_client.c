#include "demo.h"

void demo_client_send_handler(char* cmdline){}

void demo_client_hb_handler(char* cmdline) {
	if(demo_client < 0){
		printf("Demo Client is not started..\n");
		return;
	}
	dx_packet_send_heartbeat(demo_client, 0);
}

void demo_client_event_send_handler(char* cmdline){}
void demo_client_file_handler(char* cmdline){}
void demo_client_stop_handler(char* cmdline){}

