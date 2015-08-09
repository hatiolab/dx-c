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

#include <stdlib.h>     // For malloc, free
#include <string.h>     // For bzero, bcopy
#include <unistd.h>     // For close
#include <sys/socket.h>   // For connect
#include <netinet/tcp.h>  // TCP_NODELAY, TCP_QUICKACK
#include <netdb.h>      // For gethostbyname
#include <fcntl.h>      // For fcntl
#include <errno.h>      // For errno
#include <sys/epoll.h>    // For EPOLLIN

#include "dx.h"

#include "dx_debug_assert.h"
#include "dx_debug_malloc.h"

#include "dx_util_log.h"
#include "dx_util_buffer.h"

#include "dx_event_mplexer.h"
#include "dx_net_packet_io.h"
#include "dx_net_client.h"

typedef struct dx_client_context dx_client_context_t;

int dx_client_connect(char* hostname, uint16_t port) {
  struct sockaddr_in serv_addr;
  struct hostent* server;
  int rcvbufsize = DX_SOCKET_BUF_SIZE;
  int sndbufsize = DX_SOCKET_BUF_SIZE;
  int flags, yes = 1;

  int fd = socket(AF_INET, SOCK_STREAM, 0);

  if(fd < 0) {
    perror("Client - socket() error");
    exit(EXIT_FAILURE);
  }

  /* Set Receive Buffer Size */
  setsockopt(fd, SOL_SOCKET, SO_RCVBUF, &rcvbufsize, sizeof(rcvbufsize));
  setsockopt(fd, SOL_SOCKET, SO_SNDBUF, &sndbufsize, sizeof(sndbufsize));

  setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &yes, sizeof(yes));
  yes = 1;
  setsockopt(fd, IPPROTO_TCP, TCP_QUICKACK, &yes, sizeof(yes));
  yes = 1;
  setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, &yes, sizeof(yes));

  /* Set socket to non-blocking mode */
  flags = fcntl(fd, F_GETFL);
  fcntl(fd, F_SETFL, flags | O_NONBLOCK);

  server = gethostbyname(hostname);
  if(NULL == server) {
    fprintf(stderr, "Client - no such host(%s)\n", hostname);
    exit(EXIT_FAILURE);
  }

  bzero((char*)&serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  bcopy((char*)server->h_addr, (char*)&serv_addr.sin_addr.s_addr, server->h_length);
  serv_addr.sin_port = htons(port);

  if(connect(fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
    if(errno != EINPROGRESS) {
      perror("Client - connect() error");
      exit(EXIT_FAILURE);
    }
  }

  return fd;
}

int dx_client_writable_handler(dx_event_context_t* context) {
  if(context->plist_writing == NULL) {
    /*
     * TODO 여기에서 커넥션이 연결되었다는 콜백을 호출해준다.
     */

    dx_mod_event_context(context, EPOLLIN);
    dx_packet_send_event_u32(context->fd, DX_EVT_CONNECT, 0);

    return 0;
  } else {
    int nwrite = dx_write_by_poller(context);
    if(nwrite < 0 && errno != EAGAIN) {
      perror("Server write() error");
      close(context->fd);
      dx_del_event_context(context);
      return -2;
    }

    return nwrite;
  }
}

int dx_client_readable_handler(dx_event_context_t* context) {

  dx_packet_t* packet = NULL;
  int ret;
  int closed = 0;

  ret = dx_receive_packet(context, &packet);

  if(0 >= ret) {
    if(0 == ret) {
      CONSOLE("Server hung up\n");
      close(context->fd);
    } else if(0 > ret) {
      perror("Client read() error");
    }

    /* Set DISCONNECT Event on Purpose */
    dx_primitive_packet_t disconnect_packet;
    disconnect_packet.header.len = DX_PRIMITIVE_PACKET_SIZE;
    disconnect_packet.header.type = DX_PACKET_TYPE_EVENT;
    disconnect_packet.header.code = DX_EVT_DISCONNECT;
    disconnect_packet.data.u32 = 0;

    packet = (dx_packet_t*)&disconnect_packet;

    closed = 1;
  }

  /* 이번 이벤트처리시에 패킷 데이타를 다 못읽었기 때문에, 다음 이벤트에서 계속 진행하도록 리턴함 */
  if(packet == NULL)
    return 0;

  /* 서버로부터 받은 메시지로 완성된 패킷을 핸들러(사용자 로직)로 보내서 처리한다. */
  ((dx_client_event_handler)context->user_handler)(context, packet);

  if(closed == 1)
    dx_del_event_context(context);

  return 0;
}

int dx_client_start(char* hostname, int port, dx_client_event_handler handler) {
  dx_event_context_t* pcontext;
  int fd;

  fd = dx_client_connect(hostname, port);

  pcontext = dx_event_context_create();
  pcontext->fd = fd;
  pcontext->readable_handler = dx_client_readable_handler;
  pcontext->writable_handler = dx_client_writable_handler;
  pcontext->error_handler = NULL;
  pcontext->pbuf_reading = NULL;

  pcontext->user_handler = handler;

  dx_add_event_context(pcontext, EPOLLIN | EPOLLOUT);

  return fd;
}
