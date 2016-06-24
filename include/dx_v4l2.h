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

#ifndef __DX_V4L2_H
#define __DX_V4L2_H

int dx_v4l2_print_caps(int fd);
int dx_v4l2_init_mmap(int fd, uint8_t** buffer);
int dx_v4l2_capture_image(int fd, uint8_t* buffer);
int dx_v4l2_open_device(char* dev_name, int* fd);
int dx_v4l2_close_device(int fd);

#endif /* __DX_V4L2_H */
