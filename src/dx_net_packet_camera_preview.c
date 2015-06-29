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

#include "dx_net_packet_file.h"

#include <stdio.h>    // For FILE, fopen, fclose
#include <stddef.h>   // For NULL
#include <string.h>   // For memcpy
#include <sys/socket.h>
#include <stdlib.h>   // For malloc
#include <fcntl.h>    // For read, write
#include <stdint.h>   // For uint32_t, ...
#include <sys/stat.h> // For stat
#include <dirent.h>   // For DIR, opendir, ...
#include <dx_net_packet_movie_playback.h>
#include <errno.h>    // For errno

#include "dx.h"

#include "dx_debug_assert.h"
#include "dx_debug_malloc.h"

#include "dx_file_movie.h"

#include "dx_net_packet.h"
#include "dx_net_packet_io.h"
#include "dx_net_packet_camera_preview.h"

int dx_packet_send_camera_get_info(int fd, char* camera_id) {
	return 0;
}

int dx_packet_send_camera_info(int fd, char* camera_id) {
	return 0;
}

int dx_packet_send_camera_frame(int fd, char* camera_id) {
	return 0;
}

int dx_packet_send_camera_start(int fd, char* camera_id) {
	return 0;
}

int dx_packet_send_camera_stop(int fd, char* camera_id) {
	return 0;
}
