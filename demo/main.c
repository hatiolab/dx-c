#include "demo.h"

int dx_server = -1;
int dx_client = -1;

int quit = 0;

int demo_console_handler(dx_event_context_t* context);

dx_console_menu_t demo_console_menus[]  = {
	{1, "start", "You can start servers ..", demo_start_menu_handler},
		{10, "discovery-server", "start discovery-listener", demo_start_descovery_server_handler},
		{11, "discovery-client", "start discovery-requester", demo_start_descovery_client_handler},
		{12, "server", "start server", demo_start_server_handler},
		{13, "client", "start client", demo_start_client_handler},
	{2, "server", "You can do something through server ..",  demo_start_menu_handler},
		{20, "send", "send some packet to clients",  demo_server_send_handler},
		{21, "hb", "send heartbeat to clients",  demo_server_hb_handler},
		{22, "event", "send some events to clients",  demo_client_event_send_handler},
		{23, "file", "send a file to clients", demo_server_file_handler},
		{24, "stop", "stop server", demo_server_stop_handler},
	{3, "client", "You can do something through client ..",  demo_start_menu_handler},
		{30, "send", "send some packet to server",  demo_client_send_handler},
		{31, "hb", "send heartbeat to server",  demo_client_hb_handler},
		{32, "event", "send some events to server",  demo_client_event_send_handler},
		{33, "file", "send a file to server",  demo_client_file_handler},
		{34, "stop", "stop client",  demo_client_stop_handler},
	{0},
};

int main() {

	dx_event_mplexer_create();

	dx_console_start(demo_console_handler, demo_console_menus);

	/* Big Loop */
	while(!quit) {
		dx_event_mplexer_poll(-1);
	}

	dx_event_mplexer_destroy();

	CHKMEM();

	return 0;
}
