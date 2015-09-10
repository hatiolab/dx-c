#include "demo.h"

void demo_server_send_handler(char* cmdline){}
void demo_server_hb_handler(char* cmdline){}
void demo_server_file_handler(char* cmdline){}
void demo_server_stop_handler(char* cmdline){
	close(demo_client_fd_in_server);
}

void demo_server_event_send_handler(char* cmdline){
	dx_packet_send_event_u32(demo_client_fd_in_server, 100, 1000);
}

