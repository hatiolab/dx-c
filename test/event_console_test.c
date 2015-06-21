#include <stdio.h>
#include <stdlib.h>

#include "dx_debug_assert.h"
#include "dx_debug_malloc.h"

#include "dx_util_buffer.h"
#include "dx_util_trim.h"

#include "dx_event_mplexer.h"

#include "dx_net_client.h"
#include "dx_net_dgram.h"
#include "dx_net_discovery.h"
#include "dx_net_packet.h"
#include "dx_net_packet_file.h"

#include "dx.h"
#include "dx_console_usage.h"
#include "dx_console.h"

#define DISCOVERY_SERVICE_PORT	3456

/* TODO fixme to find client file descriptors */
int last_client_fd;

#define	TO_SERVER dx_client_get_fd()
#define TO_CLIENT	last_client_fd

dx_dgram_context_t* dx_client;
dx_dgram_context_t* dx_server;

int send_stream(int fd);
int dx_console_handler(dx_event_context_t* context);

void event_console_test() {
	dx_event_mplexer_create();

	dx_console_start(dx_console_handler);

	dx_event_mplexer_poll();

	CHKMEM();
}

int dx_console_handler(dx_event_context_t* context) {
    char buf[128];

    bzero(buf, 128);

    ssize_t nbytes = read(context->fd, buf, sizeof(buf));
    if(0 == nbytes) {
        printf("Console hung up\n");
        return -1;
    } else if(0 > nbytes) {
        perror("Console read() error");
        close(context->fd);
        dx_del_event_context(context);
        return -2;
    }

    switch(buf[0]) {
    case	'f':
    	dx_packet_get_filelist(TO_SERVER, trim(&buf[1]));
    	break;
    case	'F':
    	dx_packet_send_filelist(TO_CLIENT, trim(&buf[1]));
    	break;
    case	'g':
    	dx_packet_get_file(TO_SERVER, trim(&buf[1]), 0, DX_FILE_PARTIAL_MAX_SIZE - 1);
    	break;
    case	'G':
//    	dx_packet_send_file(TO_CLIENT, trim(&buf[1]));
    	break;
    case    'd':
    case    'D':
//        dx_drive_start(0);
        dx_server = dx_discovery_server_start(DISCOVERY_SERVICE_PORT);
        break;
    case    'h':
    case    'H':
    	dx_client = dx_discovery_client_start(0);
        break;
    case    'v':
    	dx_discovery_send_broadcast(dx_client, DISCOVERY_SERVICE_PORT);
		break;
    case    'b':
    case    'B':
    	dx_packet_send_heartbeat(dx_client_get_fd(), 0); /* Host to Drive */
        break;
    case    'x':
    case    'X':
    	dx_client_destroy();
        break;
    case    'k':
    case    'K':
    	close(last_client_fd);
        break;
    case    'e':
    case    'E':
        printf("Event : \n");
        break;
    case    'p':
    case    'P':
        print_usage();
        break;
    case    'Q':
    case    'q':
    	dx_event_mplexer_kill(-1);
        break;
    default:
        printf("Unknown Command : %c\n", buf[0]);
        break;
    }

    return 0;
}

int send_stream(int fd) {
	int i = 0;
	int j = 0;
	int k = 0;
	int l = 0;
	uint8_t* data = (uint8_t*)MALLOC(800*480*2);

	for(l = 0;l < 100;l++) {
		for(i = 0;i < 480;i++) {
			for(j = 0;j < 800;j++) {
				data[(i * 480 + j) * 2] = (uint8_t)k;
				data[(i * 480 + j) * 2 + 1] = (uint8_t)k++;
			}
		}

		dx_packet_send_stream(fd, DX_STREAM, 0, data, 800*480*2);
	}

	FREE(data);

	return 0;
}
