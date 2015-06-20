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

#include "../include/dx_net_client.h"

#include <stdlib.h>			// For malloc, free
#include <string.h>			// For bzero, bcopy
#include <unistd.h>			// For close
#include <sys/socket.h> 	// For connect
#include <netinet/tcp.h>	// TCP_NODELAY, TCP_QUICKACK
#include <netdb.h>			// For gethostbyname
#include <fcntl.h>			// For fcntl
#include <errno.h>			// For errno
#include "../include/dx_assert.h"

typedef struct dx_client_context dx_client_context_t;

struct dx_client_context {
	int 		socket_fd;
} *__occ;

int dx_client_get_fd() {
	return __occ->socket_fd;
}

int dx_client_create() {
	if(!__occ)
		__occ = (dx_client_context_t*)malloc(sizeof(dx_client_context_t));
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

	__occ->socket_fd = socket(AF_INET, SOCK_STREAM, 0);

	if(__occ->socket_fd < 0) {
		perror("Client - socket() error");
		exit(1);
	}

	/* Set Receive Buffer Size */
	setsockopt(__occ->socket_fd, SOL_SOCKET, SO_RCVBUF, &rcvbufsize, sizeof(rcvbufsize));
	setsockopt(__occ->socket_fd, SOL_SOCKET, SO_SNDBUF, &sndbufsize, sizeof(sndbufsize));

	setsockopt(__occ->socket_fd, IPPROTO_TCP, TCP_NODELAY, &yes, sizeof(yes));
	yes = 1;
	setsockopt(__occ->socket_fd, IPPROTO_TCP, TCP_QUICKACK, &yes, sizeof(yes));

	/* Set socket to non-blocking mode */
	flags = fcntl(__occ->socket_fd, F_GETFL);
	fcntl(__occ->socket_fd, F_SETFL, flags | O_NONBLOCK);

	server = gethostbyname(hostname);
	if(NULL == server) {
		fprintf(stderr, "Client - no such host(%s)\n", hostname);
		exit(0);
	}

	bzero((char*)&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char*)server->h_addr, (char*)&serv_addr.sin_addr.s_addr, server->h_length);
	serv_addr.sin_port = htons(port);

	if(connect(__occ->socket_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
		if(errno != EINPROGRESS) {
			perror("Client - connect() error");
			exit(1);
		}
	}

	return 0;
}

int dx_client_destroy() {
	if(!__occ)
		return 0;

	if(__occ->socket_fd)
		close(__occ->socket_fd);

	free(__occ);
    __occ = NULL;

	return 0;
}
