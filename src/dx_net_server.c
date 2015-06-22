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

#include <stdio.h>			// For printf, ..
#include <stdlib.h>			// For exit, ..
#include <unistd.h>			// For STDIN_FILENO, close
#include <linux/types.h>	// For __u8, __u16, ...
#include <netinet/in.h>		// For uint16_t, ...
#include <netinet/tcp.h>	// TCP_NODELAY, TCP_QUICKACK
#include <string.h>			// For memset
#include <fcntl.h>			// For fcntl
#include <sys/epoll.h>		// For epoll

#include "dx.h"

#include "dx_debug_assert.h"
#include "dx_debug_malloc.h"

#include "dx_util_buffer.h"

#include "dx_event_mplexer.h"
#include "dx_net_packet_io.h"
#include "dx_net_server.h"

struct dx_server_context {

	uint16_t	service_port;

	int 		server_socket_fd;
}* __dsc;

int dx_server_set_service_port(uint16_t port) {
	__dsc->service_port = port;

	return 0;
}

int dx_server_get_service_port() {
	return __dsc->service_port;
}

int dx_server_get_fd() {
	return __dsc->server_socket_fd;
}

int dx_accept_client() {
	struct sockaddr_in	clientaddr;
	socklen_t addrlen;
	int client_fd;
	int rcvbufsize = DX_SOCKET_BUF_SIZE;
	int sndbufsize = DX_SOCKET_BUF_SIZE;
	int yes = 1;

	addrlen = sizeof(clientaddr);

	client_fd = accept(__dsc->server_socket_fd, (struct sockaddr*)&clientaddr, &addrlen);

	/* Set Receive Buffer Size */
	setsockopt(client_fd, SOL_SOCKET, SO_RCVBUF, &rcvbufsize, sizeof(rcvbufsize));
	setsockopt(client_fd, SOL_SOCKET, SO_SNDBUF, &sndbufsize, sizeof(sndbufsize));

	setsockopt(client_fd, IPPROTO_TCP, TCP_NODELAY, &yes, sizeof(yes));
	yes = 1;
	setsockopt(client_fd, IPPROTO_TCP, TCP_QUICKACK, &yes, sizeof(yes));

	return client_fd;
}

int dx_server_listen() {
	struct sockaddr_in	serveraddr;
	int len = sizeof(serveraddr);

	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(__dsc->service_port);
	memset(&(serveraddr.sin_zero), '\0', 8);

	if(-1 == bind(__dsc->server_socket_fd, (struct sockaddr*)&serveraddr, sizeof(serveraddr))) {
		perror("Server-bind() error");
		exit(1);
	}

	/* TODO 여기에서 실제로 바인드된 포트를 알아낸다. */
	getsockname(__dsc->server_socket_fd, (struct sockaddr*)&serveraddr, (socklen_t *)&len);
	__dsc->service_port = ntohs(serveraddr.sin_port);

	if(-1 == listen(__dsc->server_socket_fd, 10)) {
		perror("Server-listen() error");
		exit(1);
	}

	return 1;
}

int dx_server_create() {

	int yes = 1;
	int flags;

	__dsc = MALLOC(sizeof(struct dx_server_context));

	__dsc->server_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(__dsc->server_socket_fd == -1) {
		perror("Server - socket() error");
		exit(1);
	}

	/* Set socket to non-blocking mode */
	flags = fcntl(__dsc->server_socket_fd, F_GETFL);
	fcntl(__dsc->server_socket_fd, F_SETFL, flags | O_NONBLOCK);

	if(-1 == setsockopt(__dsc->server_socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int))) {
		perror("Server-setsockopt() error");
		exit(1);
	}

	return 1;
}

int dx_server_destroy() {
	if(!__dsc->server_socket_fd)
		return 0;

	FREE(__dsc);

	__dsc = NULL;

	return 0;
}

int dx_server_acceptable_handler(dx_event_context_t* context) {
	dx_event_context_t* pcontext;
	int client_fd = dx_accept_client();

	printf("A Client tried to connect.. \n");
	pcontext = dx_event_context_create();
	pcontext->fd = client_fd;
	pcontext->readable_handler = dx_server_readable_handler;
	pcontext->writable_handler = dx_server_writable_handler;
	pcontext->error_handler = NULL;

	pcontext->pdata = context->pdata;

	dx_add_event_context(pcontext, EPOLLIN | EPOLLOUT);
	printf("A Client tried to connect.. Accepted.\n");

	return 0;
}

int dx_server_writable_handler(dx_event_context_t* context) {
	dx_mod_event_context(context, EPOLLIN);

	dx_packet_send_event_u32(context->fd, DX_EVT_CONNECT, 0);

	return 0;
}

int dx_server_readable_handler(dx_event_context_t* context) {

	dx_packet_t* packet = NULL;
	int ret;

	ret = dx_receive_packet(context, &packet);

	if(0 == ret) {
		printf("Client hung up\n");
		dx_del_event_context(context);
		return -1;
	} else if(0 > ret) {
		perror("Server read() error");
		close(context->fd);
		dx_del_event_context(context);
		return -2;
	}

	/* 이번 이벤트처리시에 패킷 데이타를 다 못읽었기 때문에, 다음 이벤트에서 계속 진행하도록 리턴함 */
	if(packet == NULL)
		return 0;

	/* 받은 메시지로 완성된 패킷을 핸들러(사용자 로직)로 보내서 처리한다. */
	((dx_server_event_handler)context->pdata)(context, packet);

	return 0;
}

int dx_server_start(int port, dx_server_event_handler handler) {
	dx_event_context_t* pcontext;

	/* create server */
	dx_server_create();

	dx_server_set_service_port(port);
	dx_server_listen();

	pcontext = dx_event_context_create();
	pcontext->fd = dx_server_get_fd();
	pcontext->readable_handler = dx_server_acceptable_handler;
	pcontext->writable_handler = NULL;
	pcontext->error_handler = NULL;

	pcontext->pdata = handler;

	dx_add_event_context(pcontext, EPOLLIN);

	return 0;
}
