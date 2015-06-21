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

#include "../include/dx_net_dgram.h"

#include <stdio.h>			// For printf, ..
#include <stdlib.h>			// For exit, ..
#include <unistd.h>			// For STDIN_FILENO, close
#include <linux/types.h>	// For __u8, __u16, ...
#include <netinet/in.h>		// For uint16_t, ...
#include <string.h>			// For memset
#include <fcntl.h>			// For fcntl

#include "../include/dx_debug_assert.h"

int dx_dgram_set_service_port(dx_dgram_context_t* odc, uint16_t port) {
	odc->service_port = port;

	return 0;
}

int dx_dgram_get_service_port(dx_dgram_context_t* odc) {
	return odc->service_port;
}

int dx_dgram_get_fd(dx_dgram_context_t* odc) {
	return odc->socket_fd;
}

int dx_dgram_listen(dx_dgram_context_t* odc) {
	struct sockaddr_in	serveraddr;
	int len = sizeof(serveraddr);

	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(odc->service_port);
	memset(&(serveraddr.sin_zero), '\0', 8);

	if(-1 == bind(odc->socket_fd, (struct sockaddr*)&serveraddr, sizeof(serveraddr))) {
		perror("Server-bind() error");
		exit(1);
	}

	/* TODO 여기에서 실제로 바인드된 포트를 알아낸다. */
	getsockname(odc->socket_fd, (struct sockaddr*)&serveraddr, &len);
	odc->service_port = ntohs(serveraddr.sin_port);

	return 1;
}

dx_dgram_context_t* dx_dgram_create() {

	int yes = 1;
	int flags;
	int rcvbufsize = DX_SOCKET_BUF_SIZE;
	int sndbufsize = DX_SOCKET_BUF_SIZE;

	dx_dgram_context_t* odc = malloc(sizeof(struct dx_dgram_context));

	odc->socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if(odc->socket_fd == -1) {
		perror("Server - socket() error");
		exit(1);
	}

	/* Set socket to non-blocking mode */
	flags = fcntl(odc->socket_fd, F_GETFL);
	fcntl(odc->socket_fd, F_SETFL, flags | O_NONBLOCK);

	if(-1 == setsockopt(odc->socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int))) {
		perror("Server-setsockopt() error : SO_REUSEADDR");
		exit(1);
	}

	yes = 1;
	if(-1 == setsockopt(odc->socket_fd, SOL_SOCKET, SO_BROADCAST, &yes, sizeof(int))) {
		perror("Server-setsockopt() error : SO_BROADCAST");
		exit(1);
	}

	/* Set Receive Buffer Size */
	setsockopt(odc->socket_fd, SOL_SOCKET, SO_RCVBUF, &rcvbufsize, sizeof(rcvbufsize));
	setsockopt(odc->socket_fd, SOL_SOCKET, SO_SNDBUF, &sndbufsize, sizeof(sndbufsize));

	return odc;
}

int dx_dgram_destroy(dx_dgram_context_t* odc) {
	if(!odc->socket_fd)
		return 0;

	free(odc);

	odc = NULL;

	return 0;
}
