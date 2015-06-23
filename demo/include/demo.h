#ifndef __DEMO_H
#define __DEMO_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <string.h>
#include <strings.h>

#include "dx_debug_malloc.h"
#include "dx_debug_assert.h"

#include "dx_console.h"

#include "dx_event_mplexer.h"

#include "dx_net_discovery.h"
#include "dx_net_server.h"
#include "dx_net_client.h"
#include "dx_net_packet.h"
#include "dx_net_packet_file.h"
#include "dx_net_packet_io.h"

#include "dx.h"

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

#endif /* __DEMO_H */
