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

#include "dx_net_packet.h"

#include <stddef.h>   // For NULL
#include <string.h>   // For memcpy
#include <sys/socket.h>
#include <netinet/in.h> // For htonl, ...
#include <stdint.h>   // For uint32_t, ...
#include <errno.h>    // For errno

#include "dx.h"
#include "dx_net.h"

#include "dx_debug_assert.h"
#include "dx_debug_malloc.h"

#include "dx_net_packet_io.h"

void dx_packet_set_header(dx_packet_t* packet, uint32_t len, uint8_t type, uint8_t code, uint8_t data_type) {
	packet->header.len = htonl(len);
	packet->header.type = type;
	packet->header.code = code;
	packet->header.data_type = data_type;
}

void dx_packet_set_flag(dx_packet_t* packet, int8_t mask) {
	packet->header.flags |= mask;
}

void dx_packet_reset_flag(dx_packet_t* packet, int8_t mask) {
	packet->header.flags &= ~mask;
}

int dx_packet_send_header(int fd, int type, int code) {
  dx_packet_t* packet;
  uint32_t  len;
  int ret;

  len = DX_PACKET_HEADER_SIZE;

  packet = (dx_packet_t*)MALLOC(len);

  dx_packet_set_header((dx_packet_t*)packet, len, type, code, DX_DATA_TYPE_NONE);

  ret = dx_write(fd, packet, len, 0);

  return ret;
}

int dx_packet_send_primitive(int fd, int type, int code, dx_primitive_data_t data) {
  dx_primitive_packet_t* packet;
  uint32_t  len;
  int ret;

  len = DX_PRIMITIVE_PACKET_SIZE;

  packet = (dx_primitive_packet_t*)MALLOC(len);

  dx_packet_set_header((dx_packet_t*)packet, len, type, code, DX_DATA_TYPE_PRIMITIVE);

  packet->data = data;

  ret = dx_write(fd, packet, DX_PRIMITIVE_PACKET_SIZE, 0);

  return ret;
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
  int ret;

  packet = (dx_u8_array_packet_t*)MALLOC(len);

  dx_packet_set_header((dx_packet_t*)packet, len, type, code, DX_DATA_TYPE_U8_ARRAY);

  packet->array.len = htonl(datalen);

  if(datalen)
    memcpy(&(packet->array.data), data, datalen);

  ret =dx_write(fd, packet, len, 0);

  return ret;
}

int dx_packet_send_string(int fd, int type, int code, char* data) {

  dx_string_packet_t* packet;
  int datalen = strlen(data);
  uint32_t len = DX_STRING_PACKET_SIZE(datalen);
  int ret;

  packet = (dx_string_packet_t*)MALLOC(len);

  dx_packet_set_header((dx_packet_t*)packet, len, type, code, DX_DATA_TYPE_STRING);

  packet->array.len = htonl(datalen);

  if(datalen)
    memcpy(&(packet->array.data), data, datalen);

  ret = dx_write(fd, packet, len, 0);

  return ret;
}

int dx_packet_send_stream(int fd, int code, int enctype, int flags, int sequence, int8_t* data, int datalen) {
  dx_stream_packet_t* packet;
  uint32_t len = DX_STREAM_PACKET_SIZE(datalen);
  int ret;
  unsigned long long ts;

  dx_clock_get_abs_msec(&ts);

  packet = (dx_stream_packet_t*)MALLOC(len);

  dx_packet_set_header((dx_packet_t*)packet, len, DX_PACKET_TYPE_STREAM, code, DX_DATA_TYPE_STREAM);

  packet->data.type = htonl(enctype);
  packet->data.len = htonl(datalen);
  packet->data.flags = htons(flags);
  packet->data.sequence = htonl(sequence);
  packet->data.ts = htonll(ts);
  packet->header.flags = DX_PACKET_FLAG_DISCARDABLE;

  if(datalen)
    memcpy(&(packet->data.data), data, datalen);

  ret = dx_write(fd, packet, len, 1 /* discardable */);

  return ret;
}
