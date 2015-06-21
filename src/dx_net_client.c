// Copyright (c) 2015 - 2015 All Right Reserved, http://hatiolab.com
//
// This source is subject to the ImageNext Permissive License.
// Please see the License.txt file for more information.
// All other rights reserved.
//
// THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WIHOUT WARRANTY OF ANY
// KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//

#include <stdlib.h>			// For malloc, free
#include <string.h>			// For bzero, bcopy
#include <unistd.h>			// For close
#include <sys/socket.h> 	// For connect
#include <netinet/tcp.h>	// TCP_NODELAY, TCP_QUICKACK
#include <netdb.h>			// For gethostbyname
#include <fcntl.h>			// For fcntl
#include <errno.h>			// For errno
#include <sys/epoll.h>			// For EPOLLIN

#include "dx.h"

#include "dx_net_client.h"
#include "dx_event_mplexer.h"
#include "dx_util_buffer.h"

#include "dx_debug_assert.h"
#include "dx_debug_malloc.h"

typedef struct dx_client_context dx_client_context_t;

struct dx_client_context {
	int 		socket_fd;
} *__dcc;

int dx_client_get_fd() {
	return __dcc->socket_fd;
}

int dx_client_create() {
	if(!__dcc)
		__dcc = (dx_client_context_t*)MALLOC(sizeof(dx_client_context_t));
	else
		return -1;

	return 0;
}

int	dx_client_connect(char* hostname, uint16_t port) {
	struct sockaddr_in serv_addr;
	struct hostent* server;
	int rcvbufsize = DX_SOCKET_BUF_SIZE;
	int sndbufsize = DX_SOCKET_BUF_SIZE;
	int flags, yes = 1;

	__dcc->socket_fd = socket(AF_INET, SOCK_STREAM, 0);

	if(__dcc->socket_fd < 0) {
		perror("Client - socket() error");
		exit(1);
	}

	/* Set Receive Buffer Size */
	setsockopt(__dcc->socket_fd, SOL_SOCKET, SO_RCVBUF, &rcvbufsize, sizeof(rcvbufsize));
	setsockopt(__dcc->socket_fd, SOL_SOCKET, SO_SNDBUF, &sndbufsize, sizeof(sndbufsize));

	setsockopt(__dcc->socket_fd, IPPROTO_TCP, TCP_NODELAY, &yes, sizeof(yes));
	yes = 1;
	setsockopt(__dcc->socket_fd, IPPROTO_TCP, TCP_QUICKACK, &yes, sizeof(yes));

	/* Set socket to non-blocking mode */
	flags = fcntl(__dcc->socket_fd, F_GETFL);
	fcntl(__dcc->socket_fd, F_SETFL, flags | O_NONBLOCK);

	server = gethostbyname(hostname);
	if(NULL == server) {
		fprintf(stderr, "Client - no such host(%s)\n", hostname);
		exit(0);
	}

	bzero((char*)&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char*)server->h_addr, (char*)&serv_addr.sin_addr.s_addr, server->h_length);
	serv_addr.sin_port = htons(port);

	if(connect(__dcc->socket_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
		if(errno != EINPROGRESS) {
			perror("Client - connect() error");
			exit(1);
		}
	}

	return 0;
}

int dx_client_destroy() {
	if(!__dcc)
		return 0;

	if(__dcc->socket_fd)
		close(__dcc->socket_fd);

	FREE(__dcc);
    __dcc = NULL;

	return 0;
}

int dx_client_writable_handler(dx_event_context_t* context) {
	if(context->fd)
		dx_mod_event_context(context, EPOLLIN);

	return 0;
}

int dx_client_readable_handler(dx_event_context_t* context) {

	dx_packet_t* packet = NULL;
	int ret;

	ret = dx_receive_packet(context, &packet);

	if(0 >= ret) {
		if(0 == ret) {
			printf("Server hung up\n");
		} else if(0 > ret) {
			perror("Client read() error");
			close(context->fd);
		}

		dx_del_event_context(context);
		dx_client_destroy();

		/* Generate DISCONNECT Event on Purpose */
		packet = (dx_packet_t*)malloc(DX_PRIMITIVE_PACKET_SIZE);
		packet->header.len = DX_PRIMITIVE_PACKET_SIZE;
		packet->header.type = DX_PACKET_TYPE_EVENT;
		packet->header.code = DX_EVT_DISCONNECT;
		((dx_primitive_packet_t*)packet)->data.u32 = 0;
		/* Generate DISCONNECT Event on Purpose */
	}

	/* 이번 이벤트처리시에 패킷 데이타를 다 못읽었기 때문에, 다음 이벤트에서 계속 진행하도록 리턴함 */
	if(packet == NULL)
		return 0;

//	switch(packet->header.type) {
//	case DX_PACKET_TYPE_HB : /* Heart Beat */
//		dx_client_handler_hb(context->fd, packet);
//		break;
//	case DX_PACKET_TYPE_GET_SETTING	: /* Get Setting */
//		dx_client_handler_get_setting(context->fd, packet);
//		break;
//	case DX_PACKET_TYPE_SET_SETTING : /* Set Setting */
//		dx_client_handler_set_setting(context->fd, packet);
//		break;
//	case DX_PACKET_TYPE_GET_STATE : /* Get State */
//		dx_client_handler_get_state(context->fd, packet);
//		break;
//	case DX_PACKET_TYPE_SET_STATE : /* Set State */
//		dx_client_handler_set_state(context->fd, packet);
//		break;
//	case DX_PACKET_TYPE_EVENT : /* Event */
//		dx_client_handler_event(context->fd, packet);
//		break;
//	case DX_PACKET_TYPE_COMMAND : /* Command */
//		dx_client_handler_command(context->fd, packet);
//		break;
//	case DX_PACKET_TYPE_FILE : /* File */
//		dx_client_handler_file(context->fd, packet);
//		break;
//	default:	/* Should not reach to here */
//		ASSERT("Client Event Handling.. should not reach to here.", !!0);
//		break;
//	}

	free(packet);

	return 0;
}

int dx_client_start(char* hostname, int port) {

	if(-1 == dx_client_create())
		return -1;

	printf("Start client to drive (%s) ..\n", hostname);

	dx_client_connect(hostname, port);

	dx_add_event_context(dx_client_get_fd(), EPOLLIN | EPOLLOUT, dx_client_readable_handler, dx_client_writable_handler, NULL);

	return 1;
}
