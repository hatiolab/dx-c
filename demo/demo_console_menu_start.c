#include "demo.h"

void demo_discovery_server_found_callback(char* hostname, int port) {
	/* TODO try to start client */
	CONSOLE("Server Found : %s(%d)\n", hostname, port);
	if(demo_client >= 0) {
		ERROR("Demo Client is Already Started..");
		return;
	}

	demo_client = dx_client_start(hostname, port, demo_client_event_handler);
	CONSOLE("Demo Client has just Started (%d:%s:%d) Successfully.\n", demo_client, hostname, port);
}

void demo_discovery_server_callback(int* port) {
	if(demo_server < 0) {
		*port = -1;
		return;
	}

	*port = dx_server_get_service_port(demo_server);
}

void demo_start_menu_handler(char* cmdline) {
	/* need not to do anything &*/
}

void demo_start_discovery_server_handler(char* cmdline) {
	char* param;
	int port = DEFAULT_DISCOVERY_PORT;

	if(demo_discovery_server >= 0) {
		ERROR("Discovery Server Already Started..");
		return;
	}

	if(cmdline != NULL && (param = strtok(cmdline, " \t\n\f")) != NULL)
		port = atoi(param);

	demo_discovery_server = dx_discovery_server_start(port, demo_discovery_server_callback);
	CONSOLE("Discovery Server(%d:%d) Started Successfully\n", demo_discovery_server, port);
}

void demo_start_discovery_client_handler(char* cmdline) {
	char* param;
	int port = 0;

	if(demo_discovery_client >= 0) {
		ERROR("Discovery Client Already Started..");
		return;
	}

	if(cmdline != NULL && (param = strtok(cmdline, " \t\n\f")) != NULL)
		port = atoi(param);

	demo_discovery_client = dx_discovery_client_start(port, demo_discovery_server_found_callback);
	CONSOLE("Discovery Client(%d:%d) Started Successfully\n", demo_discovery_client, port);
}

void demo_start_server_handler(char* cmdline) {
	char* param;
	int port = DEFAULT_SERVER_PORT;

	if(demo_server >= 0) {
		ERROR("Demo Server Already Started..");
		return;
	}

	if(cmdline != NULL && (param = strtok(cmdline, " \t\n\f")) != NULL)
		port = atoi(param);

	demo_server = dx_server_start(port, demo_server_event_handler);
	CONSOLE("Demo Server(%d:%d) Started Successfully\n", demo_server, dx_server_get_service_port(demo_server));
}

void demo_start_client_handler(char* cmdline) {
	char* hostname = strtok(cmdline, " \t\n\f");
	char* str_port = strtok(NULL, " \t\n\f");

	int port = DEFAULT_SERVER_PORT;

	if(demo_client >= 0) {
		ERROR("Demo Client Already Started..");
		return;
	}

	if(hostname == NULL)
		hostname = "localhost";
	if(str_port == NULL)
		port = DEFAULT_SERVER_PORT;
	else
		port = atoi(str_port);

	demo_client = dx_client_start(hostname, port, demo_client_event_handler);
	CONSOLE("Demo Client(%d:%s:%d) Started Successfully\n", demo_client, hostname, port);
}

void demo_discovery_send_handler(char* cmdline) {
	char* param;
	int port = DEFAULT_DISCOVERY_PORT;

	if(demo_discovery_client < 0) {
		ERROR("Start Discovery Client First..");
		return;
	}

	if(cmdline != NULL && (param = strtok(cmdline, " \t\n\f")) != NULL)
		port = atoi(param);

	dx_discovery_send_broadcast(demo_discovery_client, port);
}

void demo_start_all_handler(char* cmdline) {
	char* param;
	int port = DEFAULT_DISCOVERY_PORT;

	if(cmdline != NULL && (param = strtok(cmdline, " \t\n\f")) != NULL)
		port = atoi(param);

	if(demo_server < 0)
		demo_server = dx_server_start(0, demo_server_event_handler);
	if(demo_discovery_server < 0)
		demo_discovery_server = dx_discovery_server_start(port, demo_discovery_server_callback);
	if(demo_discovery_client < 0)
		demo_discovery_client = dx_discovery_client_start(0, demo_discovery_server_found_callback);

	if(demo_client < 0)
		dx_discovery_send_broadcast(demo_discovery_client, port);
}

void demo_start_all_server_handler(char* cmdline) {
	char* param;
	int port = DEFAULT_DISCOVERY_PORT;

	if(cmdline != NULL && (param = strtok(cmdline, " \t\n\f")) != NULL)
		port = atoi(param);

	if(demo_server < 0)
		demo_server = dx_server_start(0, demo_server_event_handler);
	if(demo_discovery_server < 0)
		demo_discovery_server = dx_discovery_server_start(port, demo_discovery_server_callback);
}

void demo_start_all_client_handler(char* cmdline) {
	char* param;
	int port = DEFAULT_DISCOVERY_PORT;

	if(cmdline != NULL && (param = strtok(cmdline, " \t\n\f")) != NULL)
		port = atoi(param);

	if(demo_discovery_client < 0)
		demo_discovery_client = dx_discovery_client_start(0, demo_discovery_server_found_callback);

	if(demo_client < 0)
		dx_discovery_send_broadcast(demo_discovery_client, port);
}
