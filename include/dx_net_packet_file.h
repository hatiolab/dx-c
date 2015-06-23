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

#ifndef __DX_NET_PACKET_FILE_H
#define __DX_NET_PACKET_FILE_H

#include <stdint.h> // For uint_16_t, ...
#include <time.h> // For time_t

#include "dx_net_packet.h"

/*
 * Definitions
 */

#define DX_PATH_MAX_SIZE  128
#define DX_FILE_PARTIAL_MAX_SIZE  500000L /* Large: 1M-bytes (1024*1024), Small: 15000L */

typedef struct dx_fileinfo_data {
  uint32_t  size;
  uint32_t  mtime; /* time_t => 8 bytes, so, uint32_t is used */
  uint8_t   path[DX_PATH_MAX_SIZE];
} __attribute__((__packed__)) dx_fileinfo_data_t;

typedef struct dx_fileinfo_array_data {
  uint32_t  len;
  uint8_t   path[DX_PATH_MAX_SIZE];
  dx_fileinfo_data_t fileinfo[];
} __attribute__((__packed__)) dx_fileinfo_array_data_t;

typedef struct dx_fileinfo_array_packet {
  dx_packet_header_t header;
  dx_fileinfo_array_data_t array;
} __attribute__((__packed__)) dx_fileinfo_array_packet_t;

#define DX_FILEINFO_ARRAY_PACKET_SIZE(n) (12+4+DX_PATH_MAX_SIZE+(8+DX_PATH_MAX_SIZE)*(n))

typedef struct dx_file_query {
  uint32_t  offset_begin;
  uint32_t  offset_end;
  uint8_t   path[DX_PATH_MAX_SIZE];
} __attribute__((__packed__)) dx_file_query_t;

typedef struct dx_file_query_packet {
  dx_packet_header_t  header;
  dx_file_query_t file;
} __attribute__((__packed__)) dx_file_query_packet_t;

#define DX_FILE_QUERY_PACKET_SIZE (12+8+DX_PATH_MAX_SIZE)

typedef struct dx_file_data {
  uint32_t  total_len;
  uint32_t  partial_len;
  uint32_t  offset_begin;
  uint32_t  offset_end;
  uint8_t   path[DX_PATH_MAX_SIZE];
  uint8_t   content[];
} __attribute__((__packed__)) dx_file_data_t;

typedef struct dx_file_packet {
  struct dx_packet_header header;
  struct dx_file_data file;
} __attribute__((__packed__)) dx_file_packet_t;

#define DX_FILE_PACKET_SIZE(n) (12+16+DX_PATH_MAX_SIZE+(n))

/* Functions */

int dx_packet_get_filelist(int fd, char* path);
int dx_packet_send_filelist(int fd, char* path);

int dx_packet_get_file(int fd, char* path, uint32_t begin, uint32_t end);
int dx_packet_send_file(int fd, char* path, uint32_t begin, uint32_t end);

int dx_packet_delete_file(int fd, char* path);

#endif /* DX_NET_PACKET_FILE_H */
