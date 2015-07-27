#include <stdio.h>
#include <stdlib.h>
#include <strings.h>	// For bzero
#include <fcntl.h>		// For fcntl
#include <unistd.h>		// For pipe
#include <stddef.h>		// For NULL
#include <sys/epoll.h>	// For EPOLLIN

#include "dx.h"

#include "dx_debug_assert.h"
#include "dx_debug_malloc.h"

#include "dx_util_buffer.h"
#include "dx_util_log.h"

#include "dx_event_mplexer.h"
#include "dx_net_packet.h"
#include "dx_net_discovery.h"

void discovery_test_server_callback(int* port);

void discovery_test_server_found_callback(char* hostname, int port);

void net_discovery_test() {
	int discovery_server_fd;
	int discovery_client_fd;
	int i = 0;

	/* event multiplexer start */
	dx_event_mplexer_create();

	/* discovery server start */
	discovery_server_fd = dx_discovery_server_start(0, discovery_test_server_callback);

	/* discovery client start */
	discovery_client_fd = dx_discovery_client_start(0, discovery_test_server_found_callback);

	/* Big Loop */
	while(i++ < 100) {
		dx_event_mplexer_poll(1000);

		dx_discovery_send_broadcast(discovery_client_fd, dx_dgram_get_service_port(discovery_server_fd));
	}

	dx_event_mplexer_destroy();

	CHKMEM();
}

void discovery_test_server_found_callback(char* hostname, int port) {
	CONSOLE("Server Found : %s(%d)\n", hostname, port);
	ASSERT("Port Number should be 1000\n", port == 1000);
}

void discovery_test_server_callback(int* port) {
	*port = 1000;
}
