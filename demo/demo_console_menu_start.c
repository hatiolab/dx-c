#include "demo.h"

void demo_start_menu_handler(char* cmdline){}
void demo_start_descovery_server_handler(char* cmdline){}
void demo_start_descovery_client_handler(char* cmdline){}
void demo_start_server_handler(char* cmdline){}
void demo_start_client_handler(char* cmdline){}

void demo_console_start_handler(char* cmdline) {
}


void demo_discovery_server_found_callback(char* hostname, int port) {
	/* TODO try to start client */
	printf("Server Found : %s(%d)\n", hostname, port);
	ASSERT("Port Number should be 1000\n", port == 1000);
}

void demo_discovery_server_callback(int* port) {
	int server_port = dx_server_get_service_port(dx_server);

	/* TODO try to send back response with server port - dx_server */
}

void demo_console_start_discovery_server_handler(dx_event_context_t* context, char* cmdline) {
	int port = DEFAULT_DISCOVERY_PORT;
	if(cmdline != NULL && strlen(cmdline) > 0)
		port = atoi(strtok(cmdline, " \t\n\f"));

	dx_discovery_server_start(port, demo_discovery_server_callback);
}

void demo_console_start_discovery_client_handler(dx_event_context_t* context, char* cmdline) {
	int port = DEFAULT_DISCOVERY_PORT;
	if(cmdline != NULL && strlen(cmdline) > 0)
		port = atoi(strtok(cmdline, " \t\n\f"));

	dx_discovery_client_start(port, demo_discovery_server_found_callback);
}

int demo_server_event_handler(dx_event_context_t* pcontext, dx_packet_t* packet) {
	return 0;
}

int demo_client_event_handler(dx_event_context_t* pcontext, dx_packet_t* packet) {
	return 0;
}

void demo_console_start_server_handler(dx_event_context_t* context, char* cmdline) {
	int port = DEFAULT_SERVER_PORT;
	if(cmdline != NULL && strlen(cmdline) > 0)
		port = atoi(strtok(cmdline, " \t\n\f"));

	dx_server_start(port, demo_server_event_handler);
}

void demo_console_start_client_handler(dx_event_context_t* context, char* cmdline) {
	char* hostname = strtok(cmdline, " \t\n\f");
	char* str_port = strtok(NULL, " \t\n\f");

	int port = DEFAULT_SERVER_PORT;

	if(hostname == NULL)
		hostname = "localhost";
	if(str_port == NULL)
		port = DEFAULT_SERVER_PORT;
	else
		port = atoi(str_port);

	dx_client_start(hostname, port, demo_client_event_handler);
}

