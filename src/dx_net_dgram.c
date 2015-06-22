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

#include "dx_net_dgram.h"

#include <stdio.h>			// For printf, ..
#include <stdlib.h>			// For exit, ..
#include <unistd.h>			// For STDIN_FILENO, close
#include <linux/types.h>	// For __u8, __u16, ...
#include <netinet/in.h>		// For uint16_t, ...
#include <string.h>			// For memset
#include <fcntl.h>			// For fcntl
#include <sys/epoll.h>		// For EPOLLIN

#include "dx_debug_assert.h"

int dx_dgram_readable_handler(dx_event_context_t* context);

int dx_dgram_set_service_port(dx_dgram_context_t* ddc, uint16_t port) {
	ddc->service_port = port;

	return 0;
}

int dx_dgram_get_service_port(dx_dgram_context_t* ddc) {
	return ddc->service_port;
}

int dx_dgram_get_fd(dx_dgram_context_t* ddc) {
	return ddc->socket_fd;
}

int dx_dgram_listen(dx_dgram_context_t* ddc) {
	struct sockaddr_in	serveraddr;
	int len = sizeof(serveraddr);

	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(ddc->service_port);
	memset(&(serveraddr.sin_zero), '\0', 8);

	if(-1 == bind(ddc->socket_fd, (struct sockaddr*)&serveraddr, sizeof(serveraddr))) {
		perror("Server-bind() error");
		exit(1);
	}

	/* TODO 여기에서 실제로 바인드된 포트를 알아낸다. */
	getsockname(ddc->socket_fd, (struct sockaddr*)&serveraddr, (socklen_t*)&len);
	ddc->service_port = ntohs(serveraddr.sin_port);

	return 1;
}

dx_dgram_context_t* dx_dgram_create() {

	int yes = 1;
	int flags;
	int rcvbufsize = DX_SOCKET_BUF_SIZE;
	int sndbufsize = DX_SOCKET_BUF_SIZE;

	dx_dgram_context_t* ddc = malloc(sizeof(struct dx_dgram_context));

	ddc->socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if(ddc->socket_fd == -1) {
		perror("Server - socket() error");
		exit(1);
	}

	/* Set socket to non-blocking mode */
	flags = fcntl(ddc->socket_fd, F_GETFL);
	fcntl(ddc->socket_fd, F_SETFL, flags | O_NONBLOCK);

	if(-1 == setsockopt(ddc->socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int))) {
		perror("Server-setsockopt() error : SO_REUSEADDR");
		exit(1);
	}

	yes = 1;
	if(-1 == setsockopt(ddc->socket_fd, SOL_SOCKET, SO_BROADCAST, &yes, sizeof(int))) {
		perror("Server-setsockopt() error : SO_BROADCAST");
		exit(1);
	}

	/* Set Receive Buffer Size */
	setsockopt(ddc->socket_fd, SOL_SOCKET, SO_RCVBUF, &rcvbufsize, sizeof(rcvbufsize));
	setsockopt(ddc->socket_fd, SOL_SOCKET, SO_SNDBUF, &sndbufsize, sizeof(sndbufsize));

	return ddc;
}

int dx_dgram_destroy(dx_dgram_context_t* ddc) {
	if(!ddc->socket_fd)
		return 0;

	free(ddc);

	ddc = NULL;

	return 0;
}

int dx_dgram_start(dx_dgram_context_t* ddc, dx_dgram_event_handler handler) {
	dx_event_context_t* pcontext;

	dx_dgram_listen(ddc);

	pcontext = dx_event_context_create();
	pcontext->fd = ddc->socket_fd;
	pcontext->readable_handler = dx_dgram_readable_handler;
	pcontext->writable_handler = NULL;
	pcontext->error_handler = NULL;

	pcontext->pdata = handler;

	dx_add_event_context(pcontext, EPOLLIN);

	return 0;
}

int dx_dgram_readable_handler(dx_event_context_t* context) {

	struct sockaddr_in peer_addr;
	dx_packet_t* packet = NULL;
	int nread;

	nread = dx_receive_dgram(context, &packet, &peer_addr);

	if(0 > nread) {
		perror("Reading Datagram - recvfrom() error");
		close(context->fd);
		dx_del_event_context(context);
		return nread;
	}

	/* 받은 메시지로 완성된 패킷을 핸들러(사용자 로직)로 보내서 처리한다. */
	((dx_dgram_event_handler)context->pdata)(context, packet, &peer_addr);

	return 0;
}
