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

#include "dx_net_dgram.h"

#include <stdio.h>      // For printf, ..
#include <stdlib.h>     // For exit, ..
#include <unistd.h>     // For STDIN_FILENO, close
#include <linux/types.h>  // For __u8, __u16, ...
#include <netinet/in.h>   // For uint16_t, ...
#include <string.h>     // For memset
#include <fcntl.h>      // For fcntl
#include <sys/epoll.h>    // For EPOLLIN

#include "dx.h"

#include "dx_debug_assert.h"
#include "dx_net_packet_io.h"

int dx_dgram_readable_handler(dx_event_context_t* context);

int dx_dgram_get_service_port(int fd) {
  struct sockaddr_in  serveraddr;
  int len = sizeof(serveraddr);

  getsockname(fd, (struct sockaddr*)&serveraddr, (socklen_t*)&len);
  return ntohs(serveraddr.sin_port);
}

int dx_dgram_listen(int fd, int port) {
  struct sockaddr_in  serveraddr;

  serveraddr.sin_family = AF_INET;
  serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
  serveraddr.sin_port = htons(port);
  memset(&(serveraddr.sin_zero), '\0', 8);

  if(-1 == bind(fd, (struct sockaddr*)&serveraddr, sizeof(serveraddr))) {
    perror("Server-bind() error");
    exit(EXIT_FAILURE);
  }

  return 1;
}

int dx_dgram_create() {

  int yes = 1;
  int flags;
  int rcvbufsize = DX_DGRAM_BUF_SIZE;
  int sndbufsize = DX_DGRAM_BUF_SIZE;

  int fd = socket(AF_INET, SOCK_DGRAM, 0);

  if(fd == -1) {
    perror("Server - socket() error");
    exit(EXIT_FAILURE);
  }

  /* Set socket to non-blocking mode */
  flags = fcntl(fd, F_GETFL);
  fcntl(fd, F_SETFL, flags | O_NONBLOCK);

  if(-1 == setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int))) {
    perror("Server-setsockopt() error : SO_REUSEADDR");
    exit(EXIT_FAILURE);
  }

  yes = 1;
  if(-1 == setsockopt(fd, SOL_SOCKET, SO_BROADCAST, &yes, sizeof(int))) {
    perror("Server-setsockopt() error : SO_BROADCAST");
    exit(EXIT_FAILURE);
  }

  /* Set Receive Buffer Size */
  setsockopt(fd, SOL_SOCKET, SO_RCVBUF, &rcvbufsize, sizeof(rcvbufsize));
  setsockopt(fd, SOL_SOCKET, SO_SNDBUF, &sndbufsize, sizeof(sndbufsize));

  return fd;
}

int dx_dgram_start(int port, dx_dgram_event_handler handler) {
  int fd = dx_dgram_create();

  dx_event_context_t* pcontext;

  dx_dgram_listen(fd, port);

  pcontext = dx_event_context_create();
  pcontext->fd = fd;
  pcontext->readable_handler = dx_dgram_readable_handler;
  pcontext->writable_handler = NULL;
  pcontext->error_handler = NULL;

  pcontext->user_handler = handler;

  dx_add_event_context(pcontext, EPOLLIN);

  return fd;
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
  ((dx_dgram_event_handler)context->user_handler)(context, packet, &peer_addr);

  return 0;
}
