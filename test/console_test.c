#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

#include "dx.h"

#include "dx_debug_assert.h"
#include "dx_debug_malloc.h"

#include "dx_util_buffer.h"
#include "dx_util_log.h"

#include "dx_event_mplexer.h"

#include "dx_net_client.h"
#include "dx_net_dgram.h"
#include "dx_net_discovery.h"
#include "dx_net_packet.h"
#include "dx_net_packet_file.h"

#include "dx_console.h"

void test_console_menu_handler(char* cmdline) {
	CONSOLE("CMD : %s\n", cmdline);
}

dx_console_menu_t test_console_menus[]  = {
	{1, "start", "", "start servers ..", NULL},
		{10, "ds", "[port]", "start discovery-listener", test_console_menu_handler},
		{11, "dc", "[port]", "start discovery-requester", test_console_menu_handler},
		{12, "server", "[port]", "start server", test_console_menu_handler},
		{13, "client", "[hostname] [port]", "start client", test_console_menu_handler},
		{14, "discovery", "[port]", "start client through discovery request", test_console_menu_handler},
		{15, "all", "[port]", "start all services on the localhost", test_console_menu_handler},
		{16, "sall", "[port]", "start all services for server", test_console_menu_handler},
		{17, "call", "[port]", "start all services for client", test_console_menu_handler},
	{2, "server", "", "do something with server ..",  NULL},
		{20, "send", "", "send some packet to clients",  test_console_menu_handler},
		{21, "hb", "", "send heartbeat to clients",  test_console_menu_handler},
		{22, "event", "", "send some events to clients",  test_console_menu_handler},
		{23, "file", "", "send a file to clients", test_console_menu_handler},
		{24, "stop", "", "stop server", test_console_menu_handler},
	{3, "client", "", "do something with client ..",  NULL},
		{30, "send", "", "send some packet to server",  test_console_menu_handler},
		{31, "hb", "", "send heartbeat to server",  test_console_menu_handler},
		{32, "event", "", "send some events to server",  test_console_menu_handler},
		{33, "file", "", "send a file to server",  test_console_menu_handler},
		{34, "stop", "", "stop client",  test_console_menu_handler},
	{4, "video", "", "do something about video/camera ..", NULL},
		{40, "file", "", "do something about video file ..", NULL},
			{400, "open", "[filename]", "open video file", test_console_menu_handler},
			{401, "info", "", "read video frames, print frame info", test_console_menu_handler},
			{402, "close", "", "close video file", test_console_menu_handler},
		{41, "camera", "", "do something about video camera ..", NULL},
	{0},
};

int console_test_quit = 0;

void test_exit_handler() {
	CONSOLE("Exit Handler works.\n");
	console_test_quit = 1;
}

void console_test() {

	dx_event_mplexer_create();

	dx_console_start(test_console_menus, test_exit_handler);

	while(console_test_quit == 0) {
		dx_event_mplexer_poll(1000);
	}

	dx_event_mplexer_destroy();

	CHKMEM();
}

