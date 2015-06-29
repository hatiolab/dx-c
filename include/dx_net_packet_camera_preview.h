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

#ifndef __DX_NET_PACKET_CAMERA_PREVIEW_H
#define __DX_NET_PACKET_CAMERA_PREVIEW_H

#include <stdint.h> // For uint_16_t, ...
#include <time.h> 	// For time_t

#include "dx_net_packet.h"

/*
 * Definitions
 */

typedef struct dx_data_camera_get_info {
	// id
} __attribute__((__packed__)) dx_data_camera_get_info_t;

typedef struct dx_data_camera_info {
	// id, resolution, framerate, color frame, encoding type
} __attribute__((__packed__)) dx_data_camera_info_t;

typedef struct dx_data_camera_frame {
	// file path, framerate, 총 playtime, 총 frame count, 현재 frame #, 현재 playtime, flags (key frame, last frame)
	// 각 트랙별 id, offset, length, encoding type
	// 각 트랙별 byte array
} __attribute__((__packed__)) dx_data_camera_frame_t;

typedef struct dx_data_camera_start {
	// file path, frame count(프레임을 몇 개나 받을 것인가?), frame rate, resolution,
} __attribute__((__packed__)) dx_data_camera_start_t;

typedef struct dx_data_camera_stop {
	// file path
} __attribute__((__packed__)) dx_data_camera_stop_t;

/* Functions */

int dx_packet_send_camera_get_info(int fd, char* camera_id);
int dx_packet_send_camera_info(int fd, char* camera_id);

int dx_packet_send_camera_frame(int fd, char* camera_id);

int dx_packet_send_camera_start(int fd, char* camera_id);
int dx_packet_send_camera_stop(int fd, char* camera_id);

#endif /* __DX_NET_PACKET_CAMERA_PREVIEW_H */
