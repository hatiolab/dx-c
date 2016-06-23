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

#ifndef __DX_CAMERA_H
#define __DX_CAMERA_H

typedef struct dx_camera_context {
	int	fd;
	uint8_t* buffer;
} dx_camera_context_t;

dx_camera_context_t* dx_camera_context_create(char* device_path);
void dx_camera_context_destroy(dx_camera_context_t* pcontext);

#endif /* __DX_CAMERA_H */
