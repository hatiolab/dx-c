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

#include "../include/dx_net_server.h"

#include <stdio.h>			// For printf, ..
#include <stdlib.h>			// For exit, ..
#include <unistd.h>			// For STDIN_FILENO, close
#include <linux/types.h>	// For __u8, __u16, ...
#include <netinet/in.h>		// For uint16_t, ...
#include <netinet/tcp.h>	// TCP_NODELAY, TCP_QUICKACK
#include <string.h>			// For memset
#include <fcntl.h>			// For fcntl

#include "../include/dx_debug_assert.h"

struct dx_server_context {

	uint16_t	service_port;

	int 		server_socket_fd;
}* __osc;

int dx_server_set_service_port(uint16_t port) {
	__osc->service_port = port;

	return 0;
}

int dx_server_get_service_port() {
	return __osc->service_port;
}

int dx_server_get_fd() {
	return __osc->server_socket_fd;
}

int dx_accept_client() {
	struct sockaddr_in	clientaddr;
	socklen_t addrlen;
	int client_fd;
	int rcvbufsize = DX_SOCKET_BUF_SIZE;
	int sndbufsize = DX_SOCKET_BUF_SIZE;
	int yes = 1;

	addrlen = sizeof(clientaddr);

	client_fd = accept(__osc->server_socket_fd, (struct sockaddr*)&clientaddr, &addrlen);

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
	serveraddr.sin_port = htons(__osc->service_port);
	memset(&(serveraddr.sin_zero), '\0', 8);

	if(-1 == bind(__osc->server_socket_fd, (struct sockaddr*)&serveraddr, sizeof(serveraddr))) {
		perror("Server-bind() error");
		exit(1);
	}

	/* TODO 여기에서 실제로 바인드된 포트를 알아낸다. */
	getsockname(__osc->server_socket_fd, (struct sockaddr*)&serveraddr, &len);
	__osc->service_port = ntohs(serveraddr.sin_port);

	if(-1 == listen(__osc->server_socket_fd, 10)) {
		perror("Server-listen() error");
		exit(1);
	}

	return 1;
}

int dx_server_create() {

	int yes = 1;
	int flags;

	__osc = MALLOC(sizeof(struct dx_server_context));

	__osc->server_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(__osc->server_socket_fd == -1) {
		perror("Server - socket() error");
		exit(1);
	}

	/* Set socket to non-blocking mode */
	flags = fcntl(__osc->server_socket_fd, F_GETFL);
	fcntl(__osc->server_socket_fd, F_SETFL, flags | O_NONBLOCK);

	if(-1 == setsockopt(__osc->server_socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int))) {
		perror("Server-setsockopt() error");
		exit(1);
	}

	return 1;
}

int dx_server_destroy() {
	if(!__osc->server_socket_fd)
		return 0;

	FREE(__osc);

	__osc = NULL;

	return 0;
}
