#ifndef __DEMO_H
#define __DEMO_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

#include "dx_debug_malloc.h"
#include "dx_debug_assert.h"

#include "dx_util_schedule.h"

#include "dx_console.h"

#include "dx_event_mplexer.h"

#include "dx_net_discovery.h"
#include "dx_net_server.h"
#include "dx_net_client.h"
#include "dx_net_packet.h"
#include "dx_net_packet_file.h"
#include "dx_net_packet_io.h"

#include "dx_file_h264.h"

#include "dx.h"

#include "omnid.h"

/*
 * Definitions
 */

#define DEFAULT_DISCOVERY_PORT	2015
#define DEFAULT_SERVER_PORT	2015

typedef void (*console_handler)(dx_event_context_t* context, char* cmdline);

typedef struct demo_menu_asso {
	char* command;
	console_handler handler;
	struct demo_menu_asso* menu_map;
}demo_menu_asso_t;

/*
 * Externs
 */
extern int demo_server;
extern int demo_client;
extern int demo_discovery_server;
extern int demo_discovery_client;

extern int quit;

extern demo_menu_asso_t start_menu_map[];
extern console_handler demo_current_handler;

void demo_start_menu_handler(char* cmdline);
void demo_start_descovery_server_handler(char* cmdline);
void demo_start_descovery_client_handler(char* cmdline);
void demo_start_server_handler(char* cmdline);
void demo_start_client_handler(char* cmdline);
void demo_descovery_send_handler(char* cmdline);
void demo_start_all_handler(char* cmdline);
void demo_start_all_server_handler(char* cmdline);
void demo_start_all_client_handler(char* cmdline);

void demo_server_send_handler(char* cmdline);
void demo_server_hb_handler(char* cmdline);
void demo_server_file_handler(char* cmdline);
void demo_server_stop_handler(char* cmdline);

void demo_client_send_handler(char* cmdline);
void demo_client_hb_handler(char* cmdline);
void demo_client_repeat_hb_handler(char* cmdline);
void demo_client_event_send_handler(char* cmdline);
void demo_client_file_handler(char* cmdline);
void demo_client_stop_handler(char* cmdline);
void demo_client_playback_start_handler(char* cmdline);
void demo_client_playback_stop_handler(char* cmdline);

void demo_video_file_open(char* cmdline);
void demo_video_file_info(char* cmdline);
void demo_video_file_close(char* cmdline);

int demo_server_event_handler(dx_event_context_t* pcontext, dx_packet_t* packet);
int demo_client_event_handler(dx_event_context_t* pcontext, dx_packet_t* packet);

#endif /* __DEMO_H */
