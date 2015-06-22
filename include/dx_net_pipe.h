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

#ifndef __DX_NET_PIPE_H
#define __DX_NET_PIPE_H

/*
 * Definitions
 */
typedef struct dx_pipe_context {

	int	read_pipe;
	int write_pipe;
} dx_pipe_context_t;

dx_pipe_context_t* dx_event_pipe_start(dx_event_handler handler);

#endif /* __DX_NET_PIPE_H */
