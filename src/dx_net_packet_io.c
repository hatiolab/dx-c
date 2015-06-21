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

#include <stddef.h>		// For NULL
#include <unistd.h>		// For ssize_t
#include <fcntl.h>		// For read, write

#include "dx_debug_assert.h"
#include "dx_debug_malloc.h"

#include "dx_util_buffer.h"

#include "dx_event_mplexer.h"

#include "dx_net_packet.h"	// For DX_PACKET_HEADER_SIZE
#include "dx_net_packet_io.h"


int dx_write(int fd, const void* buf, ssize_t sz) {
	int twrite = 0;
	int nwrite = 0;
	int flags;

	/* Set socket to blocking mode */
	flags = fcntl(fd, F_GETFL);
	fcntl(fd, F_SETFL, flags & ~O_NONBLOCK);

	while(sz - twrite > 0) {
		nwrite = write(fd, buf + twrite, sz - twrite);
		if(nwrite <= 0) {
			perror("write() error");
			break;
		}
		twrite += nwrite;
	}

	if(sz != twrite) {
		printf("dx_write() mismatch [%d - %d]\n", twrite, (int)sz);
	}

	/* Set socket to non-blocking again */
	flags = fcntl(fd, F_GETFL);
	fcntl(fd, F_SETFL, flags | O_NONBLOCK);

	return twrite;
}

int dx_read_with_block_mode(int fd, void* buf, ssize_t sz) {
	int tread = 0; /* 전체 읽은 바이트 수 */
	int nread = 0; /* read()시 읽은 바이트 수 */
	int flags;

	/* Set socket to blocking mode */
	flags = fcntl(fd, F_GETFL);
	fcntl(fd, F_SETFL, flags & ~O_NONBLOCK);

	while(sz - tread > 0) {
		nread = read(fd, buf + tread, sz - tread);
		if(nread <= 0) {
			perror("read() error");
			break;
		}
		tread += nread;
	}

	if(sz != tread) {
		printf("dx_read() mismatch [%d - %d]\n", nread, (int)sz);
	}

	/* Set socket flags to origin */
	fcntl(fd, F_SETFL, flags);

	return tread;
}

int dx_receive_packet(dx_event_context_t* pcontext, dx_packet_t** ppacket) {

	dx_buffer_t* pbuf_reading = pcontext->pbuf_reading;
	int fd = pcontext->fd;
	int nread;

	/* 각 세션별 패킷용 바이트버퍼를 찾아와서, 상태를 확인한다. */
	if(pbuf_reading == NULL || dx_buffer_getpos(pbuf_reading) == 0) {
		uint32_t read_len;
		uint32_t packet_len;

		/*
		 * 먼저 패킷 길이를 읽는다.
		 */
		nread = read(fd, &read_len, sizeof(read_len));
		if(nread <= 0) /* 오류 상황임 */
			return nread;
		ASSERT("Packet length should be read at a time.\n", nread == sizeof(read_len))

		packet_len = ntohl(read_len);

		/*
		 * 읽기 전용 버퍼를 재활용하거나, 폐기 후 새로 할당한다.
		 */
		if(pbuf_reading == NULL) {

			pbuf_reading = pcontext->pbuf_reading = dx_buffer_alloc(packet_len);
		} else if(pbuf_reading->capacity < packet_len) {

			dx_buffer_free(pbuf_reading);
			pbuf_reading = pcontext->pbuf_reading = dx_buffer_alloc(packet_len);
		} else {

			dx_buffer_clear(pbuf_reading);
			dx_buffer_setlimit(pbuf_reading, packet_len);
		}

		dx_buffer_put(pbuf_reading, &read_len, sizeof(read_len));
	}

	nread = dx_buffer_read_from(pbuf_reading, fd);

	if(dx_buffer_remains(pbuf_reading) == 0)
		*ppacket = (dx_packet_t*)&pbuf_reading->data[0];
	else
		*ppacket = NULL;

	return nread;
}
