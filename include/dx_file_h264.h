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

#ifndef __DX_FILE_h254_H
#define __DX_FILE_h254_H

#define DX_H264_FRAME_BOUNDARY 0x01000000
#define DX_H264_FRAME_MAX_SIZE (1024*1024)

/*
 * dx_h264_frame_callback
 *
 * dx_h264_frame_iterate() 에 의해 iteration될 때 호출되는 콜백 함수 정의.
 *
 * - iteration을 멈추려면, 0이 아닌 값을 리턴한다.
 */
typedef int (*dx_h264_frame_callback)(int fd, dx_buffer_t* buffer, int idx);

dx_buffer_t* dx_h264_read_frame(int fd, dx_buffer_t* bytebuffer, int frame_number);
void dx_h264_frame_iterate(int fd, dx_h264_frame_callback callback);
void dx_h264_close(int fd);
int dx_h264_open(char* path);

#endif /* __DX_FILE_h254_H */
