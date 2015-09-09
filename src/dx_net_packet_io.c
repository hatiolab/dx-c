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

#include <stddef.h>		// For NULL
#include <unistd.h>		// For ssize_t
#include <fcntl.h>		// For read, write
#include <string.h>		// For memset
#include <sys/epoll.h>	// For EPOLLIN, EPOLLOUT

#include "dx.h"

#include "dx_debug_assert.h"
#include "dx_debug_malloc.h"

#include "dx_util_buffer.h"
#include "dx_util_file.h"
#include "dx_util_log.h"

#include "dx_event_mplexer.h"

#include "dx_net_packet.h"  // For DX_PACKET_HEADER_SIZE
#include "dx_net_packet_io.h"

#include "dx_net_dgram.h" // For DX_DGRAM_MAX_PACKET_SIZE

int dx_write_by_poller(dx_event_context_t* pcontext) {
	dx_list_t* plist = pcontext->plist_writing;
	dx_buffer_t* pbuf;
	int nwrite, twrite = 0;
	dx_list_node_t* pnode;

	if (plist == NULL) {
		/* TODO 현재의 값을 가져와서 EPOLLOUT을 제거하는 방법으로 수정해야 함. */
		dx_mod_event_context(pcontext, EPOLLIN);

		return 0;
	}

	pnode = plist->head;
	if (pnode == NULL) {
		/* TODO 현재의 값을 가져와서 EPOLLOUT을 제거하는 방법으로 수정해야 함. */
		dx_mod_event_context(pcontext, EPOLLIN);

		return 0;
	}

	pbuf = (dx_buffer_t*) pnode->data;
	ASSERT("Buffer should not be NULL\n", pbuf != NULL)

	while (1) {

		if(dx_buffer_getpos(pbuf) == 0 && dx_list_size(plist) > 3) {
			/* 새로 시작하는 버퍼이면, 먼저 버퍼리스트의 크기가 일정 갯수(3개) 이상인 상황이고, discardable 이면 그냥 제거한다. */
			dx_packet_t* packet = (dx_packet_t*)dx_buffer_ppos(pbuf);
			if(packet->header.flags & DX_PACKET_FLAG_DISCARDABLE) {
				ERROR("Cut off pending discardable messages just before send..");

				goto send_buffer_done;
			}
		}

		nwrite = write(pcontext->fd, dx_buffer_ppos(pbuf),
				dx_buffer_remains(pbuf));
		if (nwrite <= 0)
			return nwrite;

		twrite += nwrite;

		dx_buffer_step_forward(pbuf, nwrite);
		if (dx_buffer_remains(pbuf) != 0)
			return nwrite;

	send_buffer_done:
		dx_list_remove(plist, pbuf);
		/* 여기에서 버퍼를 free해줄 필요없음 - destroyer에 의해서 자동 free됨 */

		pnode = plist->head;
		if (pnode == NULL) {
			/* TODO 현재의 값을 가져와서 EPOLLOUT을 제거하는 방법으로 수정해야 함. */
			dx_mod_event_context(pcontext, EPOLLIN);

			return twrite;
		}

		pbuf = (dx_buffer_t*) pnode->data;
		ASSERT("Buffer should not be NULL\n", pbuf != NULL)
	}
}

/*
 * 주의 : 파라미터로 넘겨진 버퍼는 MALLOC 된 것이어야 하며, send된 후에 자동으로 FREE된다.
 */
int dx_write(int fd, void* buf, ssize_t sz, int discardable) {
	dx_event_context_t* pcontext = dx_get_event_context(fd);
	dx_list_t* plist = NULL;
	dx_buffer_t* pbuf = NULL;

	if(pcontext == NULL) {
		FREE(buf);
		return -1;
	}

	plist = pcontext->plist_writing;

	if (plist == NULL) {
		if(dx_file_is_closed(fd)) {
			FREE(buf);
			return -1;
		}
		pcontext->plist_writing = plist = (dx_list_t*) MALLOC(
				sizeof(dx_list_t));
		dx_list_init(plist, NULL, (dx_destroyer_function) dx_buffer_free);
	}

	if(discardable && dx_list_size(plist) > 10) {
		ERROR("Cut off pending discardable messages.");
		FREE(buf);
		return 0;
	}

	pbuf = dx_buffer_wrap(buf, sz);

	dx_list_add(plist, pbuf);

	/* TODO 현재의 값을 가져와서 EPOLLOUT을 추가하는 방법으로 수정해야 함. */
	dx_mod_event_context(pcontext, EPOLLIN | EPOLLOUT);

	/* TODO 현재 쓰레드가 만일 POLLING 쓰레드라면, 깨울 필요없이, 직접 write를 하도록 한다. */
	dx_event_mplexer_wakeup();

	return 0;
}

/*
 *  파일에서 읽는 경우에 사용함.
 */
int dx_read_with_block_mode(int fd, void* buf, ssize_t sz) {
	int tread = 0; /* 전체 읽은 바이트 수 */
	int nread = 0; /* read()시 읽은 바이트 수 */
	int flags;

	/* Set socket to blocking mode */
	flags = fcntl(fd, F_GETFL);
	fcntl(fd, F_SETFL, flags & ~O_NONBLOCK);

	while (sz - tread > 0) {
		nread = read(fd, buf + tread, sz - tread);
		if (nread <= 0) {
			perror("read() error");
			break;
		}
		tread += nread;
	}

	if (sz != tread) {
		printf("dx_read() mismatch [%d - %d]\n", nread, (int) sz);
	}

	/* Set socket flags to origin */
	fcntl(fd, F_SETFL, flags);

	return tread;
}

int dx_receive_packet(dx_event_context_t* pcontext, dx_packet_t** ppacket) {

	dx_buffer_t* pbuf_reading = pcontext->pbuf_reading;
	int fd = pcontext->fd;
	int nread = 0, tread = 0;

	/* 각 세션별 패킷용 바이트버퍼를 찾아와서, 상태를 확인한다. */
	if (pbuf_reading == NULL || dx_buffer_getpos(pbuf_reading) < 4) {
		uint32_t packet_len;

		if(pbuf_reading == NULL) {
			pbuf_reading = pcontext->pbuf_reading = dx_buffer_alloc(DX_PACKET_HEADER_SIZE);
		}

		/*
		 * 먼저 패킷 길이를 읽는다.
		 * TODO 4바이트를 모두 못읽을 상황에 대한 처리가 필요하다.
		 */

		tread = dx_buffer_nread_from(pbuf_reading, 4 - dx_buffer_getpos(pbuf_reading), fd);

		if (tread <= 0) /* 오류 상황임 */
			return tread;
		if (dx_buffer_getpos(pbuf_reading) < 4) {
			*ppacket = NULL;
			return tread;
		}

		packet_len = ntohl(((dx_packet_header_t*)pbuf_reading->data)->len);

		/*
		 * 읽기 전용 버퍼를 재활용하거나, 폐기 후 새로 할당한다.
		 */
		if (pbuf_reading->capacity < packet_len) {

			pcontext->pbuf_reading = dx_buffer_alloc(packet_len);
			dx_buffer_put(pcontext->pbuf_reading, pbuf_reading->data, 4);

			dx_buffer_free(pbuf_reading);
			pbuf_reading = pcontext->pbuf_reading;
		} else {

			dx_buffer_setlimit(pbuf_reading, packet_len);
		}
	}

	nread = dx_buffer_read_from(pbuf_reading, fd);
	if(nread < 0) {
		*ppacket = NULL;
		dx_buffer_clear(pbuf_reading);
		return nread;
	}

	if (dx_buffer_remains(pbuf_reading) == 0) {
		*ppacket = (dx_packet_t*) pbuf_reading->data;
		dx_buffer_clear(pbuf_reading);
	} else {
		*ppacket = NULL;
	}

	return tread + nread;
}

int dx_receive_dgram(dx_event_context_t* pcontext, dx_packet_t** ppacket,
		struct sockaddr_in* peer_addr) {

	dx_buffer_t* pbuf_reading = pcontext->pbuf_reading;
	int fd = pcontext->fd;
	int nread;
	socklen_t slen = sizeof(struct sockaddr_in);

	/* 각 세션별 패킷용 바이트버퍼를 찾아와서, 상태를 확인한다. */
	if (pbuf_reading == NULL) {
		pbuf_reading = pcontext->pbuf_reading = dx_buffer_alloc(
				DX_DGRAM_MAX_PACKET_SIZE);
	}

	nread = recvfrom(fd, pbuf_reading->data, DX_DGRAM_MAX_PACKET_SIZE, 0,
			(struct sockaddr*) peer_addr, &slen);
	if (nread <= 0) /* 오류 상황임 */
		return nread;

	*ppacket = (dx_packet_t*) pbuf_reading->data;

	return nread;
}

/*
 * dx_send_packet_to
 *
 * Datagram Socket으로 패킷을 전송한다.
 */
int dx_send_to(int fd, dx_packet_t* packet, struct sockaddr_in* to) {

	uint32_t len = ntohl(packet->header.len);

	/* Send to broadcast */

	len = sendto(fd, packet, len, 0, (struct sockaddr*) to,
			sizeof(struct sockaddr_in));
	if (len == -1) {
		perror("Send packet to datagram error. - sendto()");
	}

	return len;
}

/*
 * dx_send_broadcast
 *
 * Datagram Socket으로 패킷을 Broadcast 한다.
 */
int dx_send_broadcast(int fd, dx_packet_t* packet, int port) {
	struct sockaddr_in broadcast_addr;

	/* Send to broadcast */

	broadcast_addr.sin_family = AF_INET;
	broadcast_addr.sin_addr.s_addr = htonl(INADDR_BROADCAST);
	broadcast_addr.sin_port = htons(port);
	memset(&(broadcast_addr.sin_zero), '\0', 8);

	return dx_send_to(fd, packet, &broadcast_addr);
}

