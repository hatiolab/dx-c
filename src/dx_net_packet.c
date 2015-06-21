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

#include "../include/dx_net_packet.h"

#include <stdio.h>		// For FILE, fopen, fclose
#include <stddef.h>		// For NULL
#include <sys/socket.h>
#include <stdlib.h>		// For malloc
#include <fcntl.h>		// For read, write
#include <stdint.h>		// For uint32_t, ...
#include <sys/stat.h>	// For stat
#include <dirent.h>		// For DIR, opendir, ...
#include <errno.h>		// For errno

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
		printf("dx_write() mismatch [%d - %d]\n", twrite, sz);
	}

	/* Set socket to non-blocking again */
	flags = fcntl(fd, F_GETFL);
	fcntl(fd, F_SETFL, flags | O_NONBLOCK);

	return twrite;
}

int dx_read(int fd, void* buf, ssize_t sz) {
	int tread = 0;
	int nread = 0;
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
		printf("dx_read() mismatch [%d - %d]\n", nread, sz);
	}

	/* Set socket to non-blocking again */
	flags = fcntl(fd, F_GETFL);
	fcntl(fd, F_SETFL, flags | O_NONBLOCK);

	return tread;
}

int dx_packet_alloc(int fd, dx_packet_t** ppacket) {
	dx_packet_header_t* packet;
	dx_packet_header_t header;
	ssize_t	nbytes;

	nbytes = dx_read(fd, &header, DX_PACKET_HEADER_SIZE);
	if(nbytes < 1)
		return nbytes;

	header.len = ntohl(header.len);

	packet = (dx_packet_t*)MALLOC(header.len);
	memcpy(packet, &header, DX_PACKET_HEADER_SIZE);

	if(header.len > DX_PACKET_HEADER_SIZE) {

		ssize_t nread;

		nread = dx_read(fd, ((void*)packet) + DX_PACKET_HEADER_SIZE, header.len - DX_PACKET_HEADER_SIZE);

		if(nread < 1) {
			FREE(packet);
			packet = NULL;
			return nread;
		}
	}

	*ppacket = packet;

	return header.len;
}

int dx_packet_send_header(int fd, int type, int code) {
	dx_packet_t* packet;
	uint32_t	len;

	len = DX_PACKET_HEADER_SIZE;

	packet = (dx_packet_t*)MALLOC(len);
	packet->header.len = htonl(len);
	packet->header.type = type;
	packet->header.code = code;
	packet->header.data_type = DX_DATA_TYPE_NONE;

	dx_write(fd, packet, len);

	FREE(packet);

	return 0;
}

int dx_packet_send_primitive(int fd, int type, int code, dx_primitive_data_t data) {
	dx_primitive_packet_t* packet;
	uint32_t	len;

	len = DX_PRIMITIVE_PACKET_SIZE;

	packet = (dx_primitive_packet_t*)MALLOC(len);
	packet->header.len = htonl(len);
	packet->header.type = type;
	packet->header.code = code;
	packet->header.data_type = DX_DATA_TYPE_PRIMITIVE;
	packet->data = data;

	dx_write(fd, packet, DX_PRIMITIVE_PACKET_SIZE);

	FREE(packet);

	return 0;
}

int dx_packet_send_primitive_u8(int fd, int type, int code, uint8_t value) {
	dx_primitive_data_t data;

	data.u8 = value;

	return dx_packet_send_primitive(fd, type, code, data);
}

int dx_packet_send_primitive_s8(int fd, int type, int code, int8_t value) {
	dx_primitive_data_t data;

	data.s8 = value;

	return dx_packet_send_primitive(fd, type, code, data);
}

int dx_packet_send_primitive_u16(int fd, int type, int code, uint16_t value) {
	dx_primitive_data_t data;

	data.u16 = htons(value);

	return dx_packet_send_primitive(fd, type, code, data);
}

int dx_packet_send_primitive_s16(int fd, int type, int code, int16_t value) {
	dx_primitive_data_t data;

	data.s16 = htons(value);

	return dx_packet_send_primitive(fd, type, code, data);
}

int dx_packet_send_primitive_u32(int fd, int type, int code, uint32_t value) {
	dx_primitive_data_t data;

	data.u32 = htonl(value);

	return dx_packet_send_primitive(fd, type, code, data);
}

int dx_packet_send_primitive_s32(int fd, int type, int code, int32_t value) {
	dx_primitive_data_t data;

	data.s32 = htonl(value);

	return dx_packet_send_primitive(fd, type, code, data);
}

int dx_packet_send_primitive_f32(int fd, int type, int code, float value) {
	dx_primitive_data_t data;

	data.f32 = value;
	data.s32 = htonl(data.s32);

	return dx_packet_send_primitive(fd, type, code, data);
}

#define DX_PACKET_SEND_HEADER(t) dx_packet_send_##t
#define DECLARE_DX_PACKET_SEND_HEADER(t,T) \
int DX_PACKET_SEND_HEADER(t)(int fd, int code) { return dx_packet_send_header(fd, T, code); }

#define DX_PACKET_SEND_PRIMITIVE_U8(t) dx_packet_send_##t##_u8
#define DECLARE_DX_PACKET_SEND_PRIMITIVE_U8(t,T) \
int DX_PACKET_SEND_PRIMITIVE_U8(t)(int fd, int code, uint8_t v) { return dx_packet_send_primitive_u8(fd, T, code, v); }

#define DX_PACKET_SEND_PRIMITIVE_S8(t) dx_packet_send_##t##_s8
#define DECLARE_DX_PACKET_SEND_PRIMITIVE_S8(t,T) \
int DX_PACKET_SEND_PRIMITIVE_S8(t)(int fd, int code, int8_t v) { return dx_packet_send_primitive_u8(fd, T, code, v); }

#define DX_PACKET_SEND_PRIMITIVE_U16(t) dx_packet_send_##t##_u16
#define DECLARE_DX_PACKET_SEND_PRIMITIVE_U16(t,T) \
int DX_PACKET_SEND_PRIMITIVE_U16(t)(int fd, int code, uint16_t v) { return dx_packet_send_primitive_u16(fd, T, code, v); }

#define DX_PACKET_SEND_PRIMITIVE_S16(t) dx_packet_send_##t##_s16
#define DECLARE_DX_PACKET_SEND_PRIMITIVE_S16(t,T) \
int DX_PACKET_SEND_PRIMITIVE_S16(t)(int fd, int code, int16_t v) { return dx_packet_send_primitive_s16(fd, T, code, v); }

#define DX_PACKET_SEND_PRIMITIVE_U32(t) dx_packet_send_##t##_u32
#define DECLARE_DX_PACKET_SEND_PRIMITIVE_U32(t,T) \
int DX_PACKET_SEND_PRIMITIVE_U32(t)(int fd, int code, uint32_t v) { return dx_packet_send_primitive_u32(fd, T, code, v); }

#define DX_PACKET_SEND_PRIMITIVE_S32(t) dx_packet_send_##t##_s32
#define DECLARE_DX_PACKET_SEND_PRIMITIVE_S32(t,T) \
int DX_PACKET_SEND_PRIMITIVE_S32(t)(int fd, int code, int32_t v) { return dx_packet_send_primitive_s32(fd, T, code, v); }

#define DX_PACKET_SEND_PRIMITIVE_F32(t) dx_packet_send_##t##_f32
#define DECLARE_DX_PACKET_SEND_PRIMITIVE_F32(t,T) \
int DX_PACKET_SEND_PRIMITIVE_F32(t)(int fd, int code, float v) { return dx_packet_send_primitive_f32(fd, T, code, v); }

DECLARE_DX_PACKET_SEND_HEADER(heartbeat, DX_PACKET_TYPE_HB)
DECLARE_DX_PACKET_SEND_HEADER(get_setting, DX_PACKET_TYPE_GET_SETTING)
DECLARE_DX_PACKET_SEND_HEADER(get_state, DX_PACKET_TYPE_GET_STATE)

DECLARE_DX_PACKET_SEND_PRIMITIVE_U8(set_setting, DX_PACKET_TYPE_SET_SETTING)
DECLARE_DX_PACKET_SEND_PRIMITIVE_S8(set_setting, DX_PACKET_TYPE_SET_SETTING)
DECLARE_DX_PACKET_SEND_PRIMITIVE_U16(set_setting, DX_PACKET_TYPE_SET_SETTING)
DECLARE_DX_PACKET_SEND_PRIMITIVE_S16(set_setting, DX_PACKET_TYPE_SET_SETTING)
DECLARE_DX_PACKET_SEND_PRIMITIVE_U32(set_setting, DX_PACKET_TYPE_SET_SETTING)
DECLARE_DX_PACKET_SEND_PRIMITIVE_S32(set_setting, DX_PACKET_TYPE_SET_SETTING)
DECLARE_DX_PACKET_SEND_PRIMITIVE_F32(set_setting, DX_PACKET_TYPE_SET_SETTING)

DECLARE_DX_PACKET_SEND_PRIMITIVE_U8(set_state, DX_PACKET_TYPE_SET_STATE)
DECLARE_DX_PACKET_SEND_PRIMITIVE_S8(set_state, DX_PACKET_TYPE_SET_STATE)
DECLARE_DX_PACKET_SEND_PRIMITIVE_U16(set_state, DX_PACKET_TYPE_SET_STATE)
DECLARE_DX_PACKET_SEND_PRIMITIVE_S16(set_state, DX_PACKET_TYPE_SET_STATE)
DECLARE_DX_PACKET_SEND_PRIMITIVE_U32(set_state, DX_PACKET_TYPE_SET_STATE)
DECLARE_DX_PACKET_SEND_PRIMITIVE_S32(set_state, DX_PACKET_TYPE_SET_STATE)
DECLARE_DX_PACKET_SEND_PRIMITIVE_F32(set_state, DX_PACKET_TYPE_SET_STATE)

DECLARE_DX_PACKET_SEND_PRIMITIVE_U8(event, DX_PACKET_TYPE_EVENT)
DECLARE_DX_PACKET_SEND_PRIMITIVE_S8(event, DX_PACKET_TYPE_EVENT)
DECLARE_DX_PACKET_SEND_PRIMITIVE_U16(event, DX_PACKET_TYPE_EVENT)
DECLARE_DX_PACKET_SEND_PRIMITIVE_S16(event, DX_PACKET_TYPE_EVENT)
DECLARE_DX_PACKET_SEND_PRIMITIVE_U32(event, DX_PACKET_TYPE_EVENT)
DECLARE_DX_PACKET_SEND_PRIMITIVE_S32(event, DX_PACKET_TYPE_EVENT)
DECLARE_DX_PACKET_SEND_PRIMITIVE_F32(event, DX_PACKET_TYPE_EVENT)

DECLARE_DX_PACKET_SEND_PRIMITIVE_U8(command, DX_PACKET_TYPE_COMMAND)
DECLARE_DX_PACKET_SEND_PRIMITIVE_S8(command, DX_PACKET_TYPE_COMMAND)
DECLARE_DX_PACKET_SEND_PRIMITIVE_U16(command, DX_PACKET_TYPE_COMMAND)
DECLARE_DX_PACKET_SEND_PRIMITIVE_S16(command, DX_PACKET_TYPE_COMMAND)
DECLARE_DX_PACKET_SEND_PRIMITIVE_U32(command, DX_PACKET_TYPE_COMMAND)
DECLARE_DX_PACKET_SEND_PRIMITIVE_S32(command, DX_PACKET_TYPE_COMMAND)
DECLARE_DX_PACKET_SEND_PRIMITIVE_F32(command, DX_PACKET_TYPE_COMMAND)

int dx_packet_send_array_u8(int fd, int type, int code, uint8_t* data, int datalen) {
	dx_u8_array_packet_t* packet;
	uint32_t len = DX_U8_ARRAY_PACKET_SIZE(datalen);

	packet = (dx_u8_array_packet_t*)MALLOC(len);

	packet->header.len = htonl(len);
	packet->header.type = type;
	packet->header.code = code;
	packet->header.data_type = DX_DATA_TYPE_U8_ARRAY;
	packet->array.len = htonl(datalen);

	if(datalen)
		memcpy(&(packet->array.data), data, datalen);

	dx_write(fd, packet, len);

	FREE(packet);

	return 0;
}

int dx_packet_send_string(int fd, int type, int code, char* data) {
	return dx_packet_send_array_u8(fd, type, code, data, strlen(data));
	dx_u8_array_packet_t* packet;
	int datalen = strlen(data);
	uint32_t len = DX_U8_ARRAY_PACKET_SIZE(datalen);

	packet = (dx_u8_array_packet_t*)MALLOC(len);

	packet->header.len = htonl(len);
	packet->header.type = type;
	packet->header.code = code;
	packet->header.data_type = DX_DATA_TYPE_STRING;
	packet->array.len = htonl(datalen);

	if(datalen)
		memcpy(&(packet->array.data), data, datalen);

	dx_write(fd, packet, len);

	FREE(packet);

	return 0;
}

int dx_packet_send_stream(int fd, int code, int enctype, uint8_t* data, int datalen) {
	dx_stream_packet_t* packet;
	uint32_t len = DX_STREAM_PACKET_SIZE(datalen);

	packet = (dx_stream_packet_t*)MALLOC(len);

	packet->header.len = htonl(len);
	packet->header.type = DX_PACKET_TYPE_STREAM;
	packet->header.code = code;
	packet->header.data_type = DX_DATA_TYPE_STREAM;
	packet->data.type = htonl(enctype);
	packet->data.len = htonl(datalen);

	if(datalen)
		memcpy(&(packet->data.data), data, datalen);

	dx_write(fd, packet, len);

	FREE(packet);

	return 0;
}
