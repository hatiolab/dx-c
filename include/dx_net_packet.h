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

#ifndef __DX_NET_PACKET_H
#define __DX_NET_PACKET_H

#include <stdint.h>	// For uint_16_t, ...
#include <time.h>	// For time_t
#include <unistd.h>	// For ssize_t

/*
 * Definitions
 */

#define DX_PACKET_FLAG_DISCARDABLE 0x01

/* Packet Types For UDP(Datagram) */

#define DX_PACKET_TYPE_DISCOVERY		0		/* Discovery */

/* Packet Types For TCP */

#define DX_PACKET_TYPE_HB				0		/* Heart Beat */
#define DX_PACKET_TYPE_GET_SETTING		1		/* Get Setting */
#define DX_PACKET_TYPE_SET_SETTING		2		/* Set Setting */
#define DX_PACKET_TYPE_GET_STATE		3		/* Get State */
#define DX_PACKET_TYPE_SET_STATE		4		/* Set State */
#define DX_PACKET_TYPE_EVENT			5		/* Event */
#define DX_PACKET_TYPE_COMMAND			6		/* Command */
#define DX_PACKET_TYPE_FILE				7		/* File Content */
#define DX_PACKET_TYPE_STREAM			8		/* Stream Content */
#define DX_PACKET_TYPE_MOVIE			9		/* Movie Playback */

/* Data Types */

#define DX_DATA_TYPE_NONE				0		/* Empty Data */
#define DX_DATA_TYPE_PRIMITIVE			1		/* Primitive Data */
#define DX_DATA_TYPE_U8_ARRAY			2		/* U8 Array */
#define DX_DATA_TYPE_S8_ARRAY			3		/* S8 Array */
#define DX_DATA_TYPE_BYTE_ARRAY			3		/* Byte(S8) Array */
#define DX_DATA_TYPE_U16_ARRAY			4		/* U16 Array */
#define DX_DATA_TYPE_S16_ARRAY			5		/* S16 Array */
#define DX_DATA_TYPE_U32_ARRAY			6		/* U32 Array */
#define DX_DATA_TYPE_S32_ARRAY			7		/* S32 Array */
#define DX_DATA_TYPE_F32_ARRAY			8		/* F32 Array */
#define DX_DATA_TYPE_STRING				9		/* String */

#define DX_DATA_TYPE_FILEINFO			21		/* File Info Data */
#define DX_DATA_TYPE_FILEINFO_ARRAY		22		/* File Info Array */
#define DX_DATA_TYPE_FILE_PARTIAL_QUERY	23		/* File Partial Query */
#define DX_DATA_TYPE_FILE_PARTIAL		24		/* File Partial */

#define DX_DATA_TYPE_STREAM				41		/* Streaming Data */

#define DX_DATA_TYPE_MOVIE_GET_INFO		51		/* Playback - Movie Get Info */
#define DX_DATA_TYPE_MOVIE_INFO			52		/* Playback - Movie Info */
#define DX_DATA_TYPE_MOVIE_COMMAND_I	53		/* Playback - Command : Start, Resume */
#define DX_DATA_TYPE_MOVIE_COMMAND_II	54		/* Playback - Command : Stop, Pause */
#define DX_DATA_TYPE_MOVIE_FRAME		55		/* Playback - Frame for Stream */

/* Packet Structures */

typedef union dx_primitive_data {
	int8_t    	s8;
	uint8_t    	u8;
	int16_t		s16;
	uint16_t   	u16;
	int32_t		s32;
	uint32_t	u32;
	float   	f32;
	uint8_t    	bits[4];
} __attribute__((__packed__)) dx_primitive_data_t;

#define DX_PRIMITIVE_DATA_SIZE	4

typedef struct dx_u8_array_data {
    uint32_t	len;
  	uint8_t		data[];
} __attribute__((__packed__)) dx_u8_array_data_t;

#define DX_U8_ARRAY_SIZE(n)		(4+(n))

typedef struct dx_s8_array_data {
    uint32_t	len;
  	int8_t		data[];
} __attribute__((__packed__)) dx_s8_array_data_t;

#define DX_S8_ARRAY_SIZE(n)		(4+(n))

typedef dx_s8_array_data_t dx_string_data_t;

#define DX_STRING_SIZE(n)		(4+(n))

typedef struct dx_u16_array_data {
    uint32_t	len;
  	uint16_t	data[];
} __attribute__((__packed__)) dx_u16_array_data_t;

#define DX_U16_ARRAY_SIZE(n)	(4+2*(n))

typedef struct dx_s16_array_data {
    uint32_t	len;
  	int16_t		data[];
} __attribute__((__packed__)) dx_s16_array_data_t;

#define DX_S16_ARRAY_SIZE(n)	(4+2*(n))

typedef struct dx_u32_array_data {
    uint32_t	len;
  	uint32_t	data[];
} __attribute__((__packed__)) dx_u32_array_data_t;

#define DX_U32_ARRAY_SIZE(n)	(4+4*(n))

typedef struct dx_s32_array_data {
    uint32_t	len;
  	int32_t		data[];
} __attribute__((__packed__)) dx_s32_array_data_t;

#define DX_S32_ARRAY_SIZE(n)	(4+4*(n))

typedef struct dx_f32_array_data {
    uint32_t	len;
  	float		data[];
} __attribute__((__packed__)) dx_f32_array_data_t;

#define DX_F32_ARRAY_SIZE(n)	(4+4*(n))

typedef struct dx_packet_header {
    uint32_t   len;
    uint8_t    type;		/* Packet Type */
    uint8_t    code;		/* Code in Packet Type */
    uint8_t    data_type;	/* Type of Following Data Part */
    uint8_t    flags;		/* Several control flags for the packet */
} __attribute__((__packed__)) dx_packet_header_t;

#define DX_PACKET_HEADER_SIZE	8

typedef struct dx_packet {
    struct dx_packet_header header;
} dx_packet_t;

typedef dx_packet_t dx_packet_hb_t;
typedef dx_packet_t dx_packet_discovery_t;

#define DX_PACKET_HB_SIZE	8

typedef struct dx_primitive_packet {
    struct dx_packet_header header;
    union dx_primitive_data data;
} __attribute__((__packed__)) dx_primitive_packet_t;

#define DX_PRIMITIVE_PACKET_SIZE	12

typedef struct dx_u8_array_packet {
    struct dx_packet_header header;
    struct dx_u8_array_data array;
} __attribute__((__packed__)) dx_u8_array_packet_t;

#define DX_U8_ARRAY_PACKET_SIZE(n)	(12+(n))

typedef struct dx_s8_array_packet {
    struct dx_packet_header	header;
    struct dx_s8_array_data	array;
} __attribute__((__packed__)) dx_s8_array_packet_t;

#define DX_S8_ARRAY_PACKET_SIZE(n)	(12+(n))

typedef dx_s8_array_packet_t dx_string_packet_t;

#define DX_STRING_PACKET_SIZE(n)	(12+(n))

typedef struct dx_u16_array_packet {
    struct dx_packet_header		header;
    struct dx_u16_array_data	array;
} __attribute__((__packed__)) dx_u16_array_packet_t;

#define DX_U16_ARRAY_PACKET_SIZE(n)	(12+2*(n))

typedef struct dx_s16_array_packet {
    struct dx_packet_header		header;
    struct dx_s16_array_data	array;
} __attribute__((__packed__)) dx_s16_array_packet_t;

#define DX_S16_ARRAY_PACKET_SIZE(n)	(12+2*(n))

typedef struct dx_u32_array_packet {
    struct dx_packet_header		header;
    struct dx_u32_array_data	array;
} __attribute__((__packed__)) dx_u32_array_packet_t;

#define DX_U32_ARRAY_PACKET_SIZE(n)	(12+4*(n))

typedef struct dx_s32_array_packet {
    struct dx_packet_header		header;
    struct dx_s32_array_data	array;
} __attribute__((__packed__)) dx_s32_array_packet_t;

#define DX_S32_ARRAY_PACKET_SIZE(n)	(12+4*(n))

typedef struct dx_f32_array_packet {
    struct dx_packet_header		header;
    struct dx_f32_array_data	array;
} __attribute__((__packed__)) dx_f32_array_packet_t;

#define DX_F32_ARRAY_PACKET_SIZE(n)	(12+4*(n))

typedef struct dx_stream_data {
	uint32_t	len;
	uint16_t	type;
	uint16_t	flags;
	uint32_t	sequence;
	uint64_t	ts;
	uint8_t		data[];
} __attribute__((__packed__)) dx_stream_data_t;

typedef struct dx_stream_packet {
	struct dx_packet_header header;
	struct dx_stream_data data;
} __attribute__((__packed__)) dx_stream_packet_t;

#define DX_STREAM_PACKET_SIZE(n) (8+20+(n))

/* Functions */

void dx_packet_set_header(dx_packet_t* packet, uint32_t len, uint8_t type, uint8_t code, uint8_t data_type);
void dx_packet_set_flag(dx_packet_t* packet, int8_t mask);
void dx_packet_reset_flag(dx_packet_t* packet, int8_t mask);

int dx_send_broadcast(int fd, dx_packet_t* packet, int port);

int dx_packet_send_primitive(int fd, int type, int code, dx_primitive_data_t data);

int dx_packet_send_heartbeat(int fd, int code);
int dx_packet_send_get_setting(int fd, int code);
int dx_packet_send_get_state(int fd, int code);

int dx_packet_send_set_setting_u8(int fd, int code, uint8_t value);
int dx_packet_send_set_setting_s8(int fd, int code, int8_t value);
int dx_packet_send_set_setting_u16(int fd, int code, uint16_t value);
int dx_packet_send_set_setting_s16(int fd, int code, int16_t value);
int dx_packet_send_set_setting_u32(int fd, int code, uint32_t value);
int dx_packet_send_set_setting_s32(int fd, int code, int32_t value);
int dx_packet_send_set_setting_f32(int fd, int code, float value);

int dx_packet_send_set_state_u8(int fd, int code, uint8_t value);
int dx_packet_send_set_state_s8(int fd, int code, int8_t value);
int dx_packet_send_set_state_u16(int fd, int code, uint16_t value);
int dx_packet_send_set_state_s16(int fd, int code, int16_t value);
int dx_packet_send_set_state_u32(int fd, int code, uint32_t value);
int dx_packet_send_set_state_s32(int fd, int code, int32_t value);
int dx_packet_send_set_state_f32(int fd, int code, float value);

int dx_packet_send_event_u8(int fd, int code, uint8_t value);
int dx_packet_send_event_s8(int fd, int code, int8_t value);
int dx_packet_send_event_u16(int fd, int code, uint16_t value);
int dx_packet_send_event_s16(int fd, int code, int16_t value);
int dx_packet_send_event_u32(int fd, int code, uint32_t value);
int dx_packet_send_event_s32(int fd, int code, int32_t value);
int dx_packet_send_event_f32(int fd, int code, float value);

int dx_packet_send_command_u8(int fd, int code, uint8_t value);
int dx_packet_send_command_s8(int fd, int code, int8_t value);
int dx_packet_send_command_u16(int fd, int code, uint16_t value);
int dx_packet_send_command_s16(int fd, int code, int16_t value);
int dx_packet_send_command_u32(int fd, int code, uint32_t value);
int dx_packet_send_command_s32(int fd, int code, int32_t value);
int dx_packet_send_command_f32(int fd, int code, float value);

int dx_packet_send_array_u8(int fd, int type, int code, uint8_t* data, int datalen);
int dx_packet_send_string(int fd, int type, int code, char* data);

int dx_packet_send_stream(int fd, int code, int enctype, int flags, int sequence, int8_t* data, int datalen);

#endif /* DX_NET_PACKET_H */
