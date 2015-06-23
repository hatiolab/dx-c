
#include <dx.h>
#include <dx_console.h>
#include <dx_debug_malloc.h>
#include <dx_event_mplexer.h>
#include <dx_net_packet.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

/*
 * Definitions
 */
#define	TO_SERVER dx_client
#define TO_CLIENT dx_server

#define DEFAULT_DISCOVERY_PORT	2020

typedef void (*console_handler)(dx_event_context_t* context, char* cmdline);

typedef struct demo_menu_asso demo_menu_asso_t;

struct demo_menu_asso {
	char* command;
	console_handler handler;
	demo_menu_asso_t* menu_map;
};

/*
 * Externs
 */
extern int dx_server;
extern int dx_client;

extern int quit;
extern demo_menu_asso_t start_menu_map[];
extern console_handler demo_current_handler;

void demo_console_start_handler(dx_event_context_t* context, char* cmdline);
void demo_console_start_discovery_server_handler(dx_event_context_t* context, char* cmdline);
void demo_console_start_discovery_client_handler(dx_event_context_t* context, char* cmdline);
void demo_console_start_server_handler(dx_event_context_t* context, char* cmdline);
void demo_console_start_client_handler(dx_event_context_t* context, char* cmdline);

/*
 * 여기서부터
 */

demo_menu_asso_t start_menu_map[] = {
	{"discovery-server", demo_console_start_discovery_server_handler, NULL},
	{"discovery-client", demo_console_start_discovery_client_handler, NULL},
	{"server", demo_console_start_server_handler, NULL},
	{"client", demo_console_start_client_handler, NULL},
};

void demo_console_start_handler(dx_event_context_t* context, char* cmdline) {
	int i = 0;
	printf("What do you want to start ? [");
	for(i = 0;i < 4;i++)
		printf(" %s,", start_menu_map[i].command);

	printf(" ]\n");
}

void demo_console_start_discovery_server_handler(dx_event_context_t* context, char* cmdline) {
	dx_discovery_server_start();
}

void demo_console_start_discovery_client_handler(dx_event_context_t* context, char* cmdline) {

}

void demo_console_start_server_handler(dx_event_context_t* context, char* cmdline) {

}

void demo_console_start_client_handler(dx_event_context_t* context, char* cmdline) {

}

