// Copyright (c) 2015 - 2015 All Right Reserved, http://hatiolab.com
//
// This source is subject to the Hatio, Lab. Permissive License.
// Please see the License.txt file for more information.
// All other rights reserved.
//
// THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WIHOUT WARRANTY OF ANY
// KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//

#include <stdio.h>      	// For printf, ..
#include <stdlib.h>     	// For exit, ..
#include <unistd.h>     	// For STDIN_FILENO, close
#include <linux/types.h>  	// For __u8, __u16, ...
#include <netinet/in.h>   	// For uint16_t, ...
#include <netinet/tcp.h>  	// TCP_NODELAY, TCP_QUICKACK
#include <string.h>     	// For memset
#include <fcntl.h>      	// For fcntl
#include <sys/epoll.h>    	// For epoll
#include <errno.h>			// For EAGAIN

#include "dx.h"

#include "dx_debug_assert.h"
#include "dx_debug_malloc.h"

#include "dx_util_buffer.h"
#include "dx_util_log.h"

#include "dx_event_mplexer.h"
#include "dx_net_packet_io.h"
#include "dx_net_server.h"

int dx_server_get_service_port(int fd) {
	struct sockaddr_in serveraddr;
	int len = sizeof(serveraddr);

	if (fd < 0)
		return -1;

	getsockname(fd, (struct sockaddr*) &serveraddr, (socklen_t *) &len);
	return ntohs(serveraddr.sin_port);
}

int dx_accept_client(int fd) {
	struct sockaddr_in clientaddr;
	socklen_t addrlen = sizeof(struct sockaddr_in);
	int client_fd;
	int rcvbufsize = DX_SOCKET_BUF_SIZE;
	int sndbufsize = DX_SOCKET_BUF_SIZE;
	int yes = 1, flags;

	addrlen = sizeof(clientaddr);

	client_fd = accept(fd, (struct sockaddr*) &clientaddr, &addrlen);

	/* Set socket to non-blocking mode */
	flags = fcntl(client_fd, F_GETFL);
	fcntl(client_fd, F_SETFL, flags | O_NONBLOCK);

	if (-1 == setsockopt(client_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int))) {
		perror("Accepted Client Socket - setsockopt() error");
		exit(EXIT_FAILURE);
	}

	/* Set Receive Buffer Size */
	setsockopt(client_fd, SOL_SOCKET, SO_RCVBUF, &rcvbufsize, sizeof(rcvbufsize));
	setsockopt(client_fd, SOL_SOCKET, SO_SNDBUF, &sndbufsize, sizeof(sndbufsize));

	setsockopt(client_fd, IPPROTO_TCP, TCP_NODELAY, &yes, sizeof(yes));
	yes = 1;
	setsockopt(client_fd, IPPROTO_TCP, TCP_QUICKACK, &yes, sizeof(yes));
	yes = 1;
	setsockopt(client_fd, SOL_SOCKET, SO_KEEPALIVE, &yes, sizeof(yes));

	return client_fd;
}

int dx_server_listen(int fd, int port) {
	struct sockaddr_in serveraddr;

	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(port);
	memset(&(serveraddr.sin_zero), '\0', 8);

	if (-1 == bind(fd, (struct sockaddr*) &serveraddr, sizeof(serveraddr))) {
		perror("Server-bind() error");
		exit(1);
	}

	if (-1 == listen(fd, 10)) {
		perror("Server-listen() error");
		exit(1);
	}

	return fd;
}

/*
 * 서버를 생성 후에 파일디스크립터를 반환한다.
 */
int dx_server_create() {

	int yes = 1;
	int flags;
	int fd;

	fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd == -1) {
		perror("Server - socket() error");
		exit(EXIT_FAILURE);
	}

	/* Set socket to non-blocking mode */
	flags = fcntl(fd, F_GETFL);
	fcntl(fd, F_SETFL, flags | O_NONBLOCK);

	if (-1 == setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int))) {
		perror("Server-setsockopt() error");
		exit(EXIT_FAILURE);
	}

	return fd;
}

int dx_server_acceptable_handler(dx_event_context_t* server_context) {
	dx_event_context_t* pcontext;
	int client_fd = dx_accept_client(server_context->fd);

	CONSOLE("A Client tried to connect.. \n");
	pcontext = dx_event_context_create();
	pcontext->fd = client_fd;
	pcontext->readable_handler = dx_server_readable_handler;
	pcontext->writable_handler = dx_server_writable_handler;
	pcontext->error_handler = NULL;

	pcontext->user_handler = server_context->user_handler;

	dx_add_event_context(pcontext, EPOLLIN | EPOLLOUT);
	CONSOLE("........ Accepted.\n");

	return 0;
}

int dx_server_writable_handler(dx_event_context_t* context) {

	if (context->plist_writing == NULL) {

		dx_mod_event_context(context, EPOLLIN);
		dx_packet_send_event_u32(context->fd, DX_EVT_CONNECT, 0);

		return 0;
	} else {
		int nwrite = dx_write_by_poller(context);
		if (nwrite < 0 && errno != EAGAIN) {
			perror("Server write() error");
			close(context->fd);
			context->fd = -1;
			dx_del_event_context(context);
			return -2;
		}

		return nwrite;
	}
}

int dx_server_readable_handler(dx_event_context_t* context) {

	dx_packet_t* packet = NULL;
	int ret;
	int closed = 0;

	ret = dx_receive_packet(context, &packet);

	if (0 >= ret) {
		if (0 == ret) {
			CONSOLE("Client hung up\n");
			close(context->fd);
		}

		if (0 > ret) {
			perror("Server read() error");
		}

		/* Set DISCONNECT Event on Purpose */
		dx_primitive_packet_t disconnect_packet;
		disconnect_packet.header.len = DX_PRIMITIVE_PACKET_SIZE;
		disconnect_packet.header.type = DX_PACKET_TYPE_EVENT;
		disconnect_packet.header.code = DX_EVT_DISCONNECT;
		disconnect_packet.data.u32 = 0;

		packet = (dx_packet_t*) &disconnect_packet;

		closed = 1;
	}

	/* 이번 이벤트처리시에 패킷 데이타를 다 못읽었기 때문에, 다음 이벤트에서 계속 진행하도록 리턴함 */
	if (packet == NULL)
		return 0;

	/* 받은 메시지로 완성된 패킷을 핸들러(사용자 로직)로 보내서 처리한다. */
	((dx_server_event_handler) context->user_handler)(context, packet);

	if (closed == 1)
		dx_del_event_context(context);

	return 0;
}

/*
 * 서버를 생성 및 Listen 후에 event multiplexer에 등록 후 파일디스크립터를 반환한다.
 */
int dx_server_start(int port, dx_server_event_handler handler) {
	dx_event_context_t* pcontext;

	/* create server */
	int fd = dx_server_create();

	dx_server_listen(fd, port);

	pcontext = dx_event_context_create();
	pcontext->fd = fd;
	pcontext->readable_handler = dx_server_acceptable_handler;
	pcontext->writable_handler = NULL;
	pcontext->error_handler = NULL;

	pcontext->user_handler = handler;

	dx_add_event_context(pcontext, EPOLLIN);

	return fd;
}
